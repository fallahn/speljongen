#include "Gpu.hpp"
#include "Display.hpp"
#include "InterruptManager.hpp"
#include "Dma.hpp"

Gpu::Gpu(Display& display, InterruptManager& ir, Dma& dma, Ram& oamRam, bool colour)
    : m_display         (display),
    m_interruptManager  (ir),
    m_dma               (dma),
    m_oamRam            (oamRam),
    m_colour            (colour),
    m_videoRam0         (0x8000, 0x2000, false),
    m_videoRam1         (0x8000, 0x2000, false),
    m_bgPalette         (0xff68),
    m_oamPalette        (0xff6a),
    m_registers         (GpuRegister::registers),
    m_oamSearchPhase    (oamRam, m_lcdc, m_registers),
    m_transferPhase     (m_videoRam0, m_videoRam1, oamRam, display, m_lcdc, m_registers, colour),
    m_currentPhase      (&m_oamSearchPhase),
    m_lcdEnabled        (true),
    m_lcdEnableDelay    (0),
    m_ticksInLine       (0),
    m_currentMode       (Mode::OamSearch)
{

}

//public
Ram& Gpu::getVRam0() { return m_videoRam0; }

Ram& Gpu::getVRam1() { return m_videoRam1; }

bool Gpu::accepts(std::uint16_t address) const
{
    return getAddressSpace(address) != nullptr;
}

void Gpu::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    if (address == GpuRegister::registers[GpuRegister::STAT].getAddress())
    {
        setStat(value);
    }
    else
    {
        auto* space = getAddressSpace(address);
        if (space == &m_lcdc)
        {
            setLcdc(value);
        }
        else
        {
            space->setByte(address, value);
        }
    }
}

std::uint8_t Gpu::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    if (address == GpuRegister::registers[GpuRegister::STAT].getAddress())
    {
        return getStat();
    }
    if (address == GpuRegister::registers[GpuRegister::VBK].getAddress())
    {
        return m_colour ? 0xfe : 0xff;
    }
    else
    {
        auto* space = getAddressSpace(address);
        return space->getByte(address);
    }
    return 0xff;
}

Gpu::Mode Gpu::tick()
{
    if (!m_lcdEnabled)
    {
        if(m_lcdEnableDelay != -1)
        {
            if (--m_lcdEnableDelay == 0)
            {
                m_display.enableLCD();
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
        if (/*m_ticksInLine == 4 &&*/ m_currentMode == Mode::VBlank && //this will never be true because VBlank mode always resets ticksInLine
            m_registers.get(GpuRegister::registers[GpuRegister::LY]) == 153)
        {
            m_registers.put(GpuRegister::registers[GpuRegister::LY], 0);
            requestLycEqualsLyInterrupt();
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
                if (m_registers.preIncrement(GpuRegister::registers[GpuRegister::LY]) == 144)
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
                if (m_registers.preIncrement(GpuRegister::registers[GpuRegister::LY]) == 1)
                {
                    m_currentMode = Mode::OamSearch;
                    m_registers.put(GpuRegister::registers[GpuRegister::LY], 0);
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
    }

    if (oldMode == m_currentMode)
    {
        return Mode::None;
    }

    return m_currentMode;
}

std::uint16_t Gpu::getTicksInLine() const { return m_ticksInLine; }

Lcdc& Gpu::getLcdc() { return m_lcdc; }

MemoryRegisters<GpuRegister>& Gpu::getRegisters() { return m_registers; }

bool Gpu::isColour() const { return m_colour; }

ColourPalette& Gpu::getBgPalette() { return m_bgPalette; }

Gpu::Mode Gpu::getMode() const { return m_currentMode; }

bool Gpu::isLcdEnabled() const { return m_lcdEnabled; }

//private
AddressSpace* Gpu::getAddressSpace(std::uint16_t address) const
{
    if (m_videoRam0.accepts(address))
    {
        if (m_colour && (m_registers.get(GpuRegister::registers[GpuRegister::VBK]) & 1) == 1)
        {
            //eww. FFS
            return static_cast<AddressSpace*>(const_cast<Ram*>(&m_videoRam1));
        }
        return static_cast<AddressSpace*>(const_cast<Ram*>(&m_videoRam0));
    }

    if (m_oamRam.accepts(address) && !m_dma.isOamBlocked())
    {
        return static_cast<AddressSpace*>(const_cast<Ram*>(&m_oamRam));
    }

    if (m_lcdc.accepts(address))
    {
        return static_cast<AddressSpace*>(const_cast<Lcdc*>(&m_lcdc));
    }

    if (m_registers.accepts(address))
    {
        return static_cast<AddressSpace*>(const_cast<MemoryRegisters<GpuRegister>*>(&m_registers));
    }

    if (m_colour)
    {
        if (m_bgPalette.accepts(address))
        {
            return static_cast<AddressSpace*>(const_cast<ColourPalette*>(&m_bgPalette));
        }

        if (m_oamPalette.accepts(address))
        {
            return static_cast<AddressSpace*>(const_cast<ColourPalette*>(&m_oamPalette));
        }
    }
    return nullptr;
}

void Gpu::requestLcdInterrupt(std::uint8_t bit)
{
    if ((m_registers.get(GpuRegister::registers[GpuRegister::STAT]) & (1 << bit)) != 0)
    {
        m_interruptManager.requestInterrupt(Interrupt::Type::LCDC);
    }
}

void Gpu::requestLycEqualsLyInterrupt()
{
    if (m_registers.get(GpuRegister::registers[GpuRegister::LYC]) ==
        m_registers.get(GpuRegister::registers[GpuRegister::LY]))
    {
        requestLcdInterrupt(6);
    }
}

std::uint8_t Gpu::getStat() const
{
    std::uint8_t m = m_registers.get(GpuRegister::registers[GpuRegister::STAT]) |
        static_cast<std::uint8_t>(m_currentMode) |
        (m_registers.get(GpuRegister::registers[GpuRegister::LYC]) == m_registers.get(GpuRegister::registers[GpuRegister::LY])) ? (1 << 2) : 0 |
        0x80;

    return m;
}

void Gpu::setStat(std::uint8_t value)
{
    m_registers.put(GpuRegister::registers[GpuRegister::STAT], value & 0xf8);
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
    m_registers.put(GpuRegister::registers[GpuRegister::LY], 0);
    m_ticksInLine = 0;
    m_hblankPhase.start(250);
    m_currentPhase = &m_hblankPhase;
    m_currentMode = Mode::HBlank;
    m_lcdEnabled = false;
    m_lcdEnableDelay = -1;
    m_display.disableLCD();
}

void Gpu::enableLCD()
{
    m_lcdEnableDelay = 244;
}