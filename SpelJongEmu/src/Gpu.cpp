#include "Gpu.hpp"
#include "Display.hpp"
#include "InterruptManager.hpp"
#include "Dma.hpp"

#include <iostream>

Gpu::Gpu(std::vector<std::uint8_t>& storage, Display& display, InterruptManager& ir, SpeedMode& sm)
    : AddressSpace      (storage),
    m_display           (display),
    m_interruptManager  (ir),
    m_dma               (storage, sm),
    m_oamRam            (storage, 0xfe00, 0x00a0),
    m_colour            (false),
    m_videoRam0         (storage, 0x8000, 0x2000/*, false*/),
    m_videoRam1         (storage, 0x8000, 0x2000, false),
    m_lcdc              (storage),
    m_bgPalette         (storage, 0xff68),
    m_oamPalette        (storage, 0xff6a),
    m_registers         (storage, MemoryRegisters::STAT, MemoryRegisters::VBK),
    m_oamSearchPhase    (m_oamRam, m_lcdc, m_registers),
    m_transferPhase     (m_videoRam0, m_videoRam1, m_oamRam, display, m_lcdc, m_registers, m_bgPalette, m_oamPalette),
    m_currentPhase      (&m_oamSearchPhase),
    m_lcdEnabled        (true),
    m_lcdEnableDelay    (0),
    m_ticksInLine       (0),
    m_currentMode       (Mode::OamSearch),
    m_vramUpdated       (true)
{

}

//public
void Gpu::reset()
{
    m_lcdEnabled = true;
    m_lcdEnableDelay = 0;
    m_ticksInLine = 0;
    m_currentMode = Mode::OamSearch;
}

Ram& Gpu::getVRam0() { return m_videoRam0; }

Ram& Gpu::getVRam1() { return m_videoRam1; }

bool Gpu::accepts(std::uint16_t address) const
{
    return m_videoRam0.accepts(address) || m_oamRam.accepts(address) || m_registers.accepts(address) //covers DMA and LCDC
        || (m_colour && (m_bgPalette.accepts(address) || m_oamPalette.accepts(address))); 
}

void Gpu::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    switch (address)
    {
    default:
        if (m_videoRam0.accepts(address))
        {
            m_vramUpdated = true;
            if (m_colour && (m_registers.getByte(MemoryRegisters::VBK) & 1) == 1)
            {
                m_videoRam1.setByte(address, value);
                return;
            }
            m_videoRam0.setByte(address, value);
            return;
        }

        if (m_oamRam.accepts(address))
        {
            if (!m_dma.isOamBlocked())
            {
                m_oamRam.setByte(address, value);
            }
            return;
        }
        
        if (m_registers.accepts(address))
        {
            m_registers.setByte(address, value);
            return;
        }

        if (m_colour)
        {
            if (m_bgPalette.accepts(address))
            {
                m_bgPalette.setByte(address, value);
                return;
            }
            if (m_oamPalette.accepts(address))
            {
                m_oamPalette.setByte(address, value);
                return;
            }
        }

        break;
    case MemoryRegisters::STAT:
        setStat(value);
        return;
    case MemoryRegisters::DMA:
        m_dma.setByte(address, value);
        return;
    case MemoryRegisters::LCDC:
        setLcdc(value);
        return;
    }
}

std::uint8_t Gpu::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    switch (address)
    {
    default:
        if (m_videoRam0.accepts(address))
        {
            if (m_colour && (m_registers.getByte(MemoryRegisters::VBK) & 1) == 1)
            {
                return m_videoRam1.getByte(address);
            }
            return m_videoRam0.getByte(address);
        }

        if (m_oamRam.accepts(address) && !m_dma.isOamBlocked())
        {
            return m_oamRam.getByte(address);
        }

        if (m_registers.accepts(address))
        {
            return m_registers.getByte(address);
        }

        if (m_colour)
        {
            if (m_bgPalette.accepts(address))
            {
                return m_bgPalette.getByte(address);
            }
            if (m_oamPalette.accepts(address))
            {
                return m_oamPalette.getByte(address);
            }
        }
        return 0xff;
    case MemoryRegisters::STAT:
        return getStat();
    case MemoryRegisters::VBK:
        return m_colour ? 0xfe : 0xff;
    case MemoryRegisters::DMA:
        return m_dma.getByte(address);
    }
}

Gpu::Mode Gpu::tick()
{
    m_dma.tick();

    if (!m_lcdEnabled)
    {
        if (m_lcdEnableDelay != -1)
        {
            if (--m_lcdEnableDelay == 0)
            {
                m_display.clear(true);
                m_lcdEnabled = true;
            }
        }
    }
    if (!m_lcdEnabled)
    {
        return Mode::None;
    }

    auto oldMode = m_currentMode;
    m_ticksInLine++;
    if (m_currentPhase->tick())
    {
        if (m_ticksInLine == 4 && m_currentMode == Mode::VBlank &&
            m_registers.getByte(MemoryRegisters::LY) == 153)
        {
            m_registers.setByte(MemoryRegisters::LY, 0);
            requestLycEqualsLyInterrupt();
        }
    }
    else
    {
        switch (oldMode)
        {
        default: break;
        case Mode::OamSearch:
            m_currentMode = Mode::PixelTransfer;
            m_transferPhase.start(m_oamSearchPhase.getSprites());
            m_currentPhase = &m_transferPhase;
            break;
        case Mode::PixelTransfer:
            m_currentMode = Mode::HBlank;
            m_hblankPhase.start(m_ticksInLine);
            m_currentPhase = &m_hblankPhase;
            requestLcdInterrupt(3);
            break;
        case Mode::HBlank:
            m_ticksInLine = 0;
            if (m_registers.preIncrement(MemoryRegisters::LY) == 144)
            {
                m_currentMode = Mode::VBlank;
                m_vblankPhase.start();
                m_currentPhase = &m_vblankPhase;
                m_interruptManager.requestInterrupt(Interrupt::Type::VBlank);
                requestLcdInterrupt(4);
            }
            else
            {
                m_currentMode = Mode::OamSearch;
                m_oamSearchPhase.start();
                m_currentPhase = &m_oamSearchPhase;
            }
            requestLcdInterrupt(5);
            requestLycEqualsLyInterrupt();
            break;
        case Mode::VBlank:
            m_ticksInLine = 0;
            if (m_registers.preIncrement(MemoryRegisters::LY) == 1)
            {
                m_currentMode = Mode::OamSearch;
                m_registers.setByte(MemoryRegisters::LY, 0);
                m_oamSearchPhase.start();
                m_currentPhase = &m_oamSearchPhase;
                requestLcdInterrupt(5);
            }
            else
            {
                m_vblankPhase.start();
                m_currentPhase = &m_vblankPhase;
            }
            requestLycEqualsLyInterrupt();
            break;
        }
    }

    if (oldMode == m_currentMode)
    {
        return Mode::None;
    }

    return m_currentMode;
}

std::uint16_t Gpu::getTicksInLine() const { return m_ticksInLine; }

Lcdc& Gpu::getLcdc() { return m_lcdc; }

bool Gpu::isColour() const { return m_colour; }

ColourPalette& Gpu::getBgPalette() { return m_bgPalette; }

Gpu::Mode Gpu::getMode() const { return m_currentMode; }

bool Gpu::isLcdEnabled() const { return m_lcdEnabled; }

bool Gpu::vramUpdated() const
{
    if (m_vramUpdated)
    {
        m_vramUpdated = false;
        return true;
    }
    return false;
}

void Gpu::enableColour(bool enable)
{
    m_transferPhase.enableColour(enable);
    m_colour = enable;
    m_videoRam0.clear();
    m_videoRam1.clear();
}

//private
void Gpu::requestLcdInterrupt(std::uint8_t bit)
{
    if ((m_registers.getByte(MemoryRegisters::STAT) & (1 << bit)) != 0)
    {
        m_interruptManager.requestInterrupt(Interrupt::Type::LCDC);
    }
}

void Gpu::requestLycEqualsLyInterrupt()
{
    if (m_registers.getByte(MemoryRegisters::LYC) ==
        m_registers.getByte(MemoryRegisters::LY))
    {
        requestLcdInterrupt(6);
    }
}

std::uint8_t Gpu::getStat() const
{
    std::uint8_t m = m_registers.getByte(MemoryRegisters::STAT) |
        static_cast<std::uint8_t>(m_currentMode) |
        ((m_registers.getByte(MemoryRegisters::LYC) == m_registers.getByte(MemoryRegisters::LY)) ? 2 : 0) |
        0x80;
    return m;
}

void Gpu::setStat(std::uint8_t value)
{
    m_registers.setByte(MemoryRegisters::STAT, value & 0xf8);
}

void Gpu::setLcdc(std::uint8_t value)
{
    m_lcdc.set(value);
    if ((value & (1 << 7)) == 0)
    {
        disableLCD();
    }
    else
    {
        enableLCD();
    }
}

void Gpu::disableLCD()
{
    m_registers.setByte(MemoryRegisters::LY, 0);
    m_ticksInLine = 0;
    m_hblankPhase.start(250);
    m_currentPhase = &m_hblankPhase;
    m_currentMode = Mode::HBlank;
    m_lcdEnabled = false;
    m_lcdEnableDelay = -1;
    m_display.clear();
}

void Gpu::enableLCD()
{
    m_lcdEnableDelay = 244;
}