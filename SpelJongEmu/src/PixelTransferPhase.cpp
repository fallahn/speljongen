#include "PixelTransferPhase.hpp"
#include "Lcdc.hpp"
#include "ClassicPixelFifo.hpp"

PixelTransferPhase::PixelTransferPhase(Ram& vram0, Ram& vram1, Ram& oam, Display& display, Lcdc& lcdc, MemoryRegisters<GpuRegister>& registers, bool colour)
    : m_display(display),
    m_lcdc(lcdc),
    m_registers(registers),
    m_colour(colour),
    //m_fetcher(),
    m_droppedPixels(0),
    m_x(0),
    m_window(false),
    m_sprites(nullptr)
{
    if (colour)
    {
        //TODO create colour fifo
        assert(!colour); //not yet implemented!
    }
    else
    {
        m_fifo = std::make_unique<ClassicPixelFifo>(display, lcdc, registers);
    }
    m_fetcher = std::make_unique<Fetcher>(*m_fifo, vram0, vram1, oam, lcdc, registers, colour);
}

//public
void PixelTransferPhase::start(std::vector<SpritePosition>& sprites)
{
    m_sprites = &sprites;
    m_droppedPixels = 0;
    m_x = 0;
    m_window = false;

    m_fetcher->init();
    if (m_colour || m_lcdc.isBgAndWindowDisplay())
    {
        startFetchingBackground();
    }
    else
    {
        m_fetcher->disableFetching();
    }
}

bool PixelTransferPhase::tick()
{
    m_fetcher->tick();

    if (m_lcdc.isBgAndWindowDisplay() || m_colour)
    {
        if (m_fifo->length() <= 8)
        {
            return true;
        }

        if (m_droppedPixels < m_registers.get(GpuRegister::registers[GpuRegister::SCX]) % 0x08)
        {
            m_fifo->dropPixel();
            m_droppedPixels++;
            return true;
        }

        if (!m_window && m_lcdc.isWindowDisplay()
            && m_registers.get(GpuRegister::registers[GpuRegister::LY]) >= m_registers.get(GpuRegister::registers[GpuRegister::WY])
            && m_x == (m_registers.get(GpuRegister::registers[GpuRegister::WX]) - 7))
        {
            m_window = true;
            startFetchingWindow();
            return true;
        }

        if (m_lcdc.isObjDisplay())
        {
            if (m_fetcher->spriteInProgress())
            {
                return true;
            }

            bool spriteAdded = false;
            for (auto i = 0u; i < m_sprites->size(); ++i)
            {
                auto& sprite = m_sprites->at(i);
                if (sprite.getAddress() == 0) continue;

                if (m_x == 0 && sprite.getX() < 8)
                {
                    if (!spriteAdded)
                    {
                        m_fetcher->addSprite(sprite, 8 - sprite.getX(), static_cast<std::uint16_t>(i));
                        spriteAdded = true;
                    }
                    m_sprites->at(i) = {};
                }
                else if (sprite.getX() - 8 == m_x)
                {
                    if (!spriteAdded)
                    {
                        m_fetcher->addSprite(sprite, 0, static_cast<std::uint16_t>(i));
                        spriteAdded = true;
                    }
                    m_sprites->at(i) = {};
                }

                if (spriteAdded)
                {
                    return true;
                }
            }
        }
    }

    m_fifo->blitPixel();
    if (++m_x == 160)
    {
        return false;
    }
    return true;
}

//private
void PixelTransferPhase::startFetchingBackground()
{
    std::uint8_t bgX = m_registers.get(GpuRegister::registers[GpuRegister::SCX]) / 0x08;
    std::uint8_t bgY = (m_registers.get(GpuRegister::registers[GpuRegister::SCY]) + m_registers.get(GpuRegister::registers[GpuRegister::LY])) % 0x100;
    m_fetcher->startFetching(m_lcdc.getBgTileMapDisplay() + (bgY / 0x08) * 0x20, m_lcdc.getBgWindowTileData(), bgX, m_lcdc.isBgWindowTileDataSigned(), bgY % 0x08);
}

void PixelTransferPhase::startFetchingWindow()
{
    std::uint8_t winX = (m_x - m_registers.get(GpuRegister::registers[GpuRegister::WX]) + 7) / 0x08;
    std::uint8_t winY = m_registers.get(GpuRegister::registers[GpuRegister::LY]) - m_registers.get(GpuRegister::registers[GpuRegister::WY]);
    m_fetcher->startFetching(m_lcdc.getWindowTileMapDisplay() + (winY / 0x08) * 0x20, m_lcdc.getBgWindowTileData(), winX, m_lcdc.isBgWindowTileDataSigned(), winY % 0x08);
}