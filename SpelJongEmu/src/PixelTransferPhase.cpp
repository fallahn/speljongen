#include "PixelTransferPhase.hpp"
#include "Lcdc.hpp"
#include "ClassicPixelFifo.hpp"
#include "ColourPixelFifo.hpp"

PixelTransferPhase::PixelTransferPhase(Ram& vram0, Ram& vram1, Ram& oam, Display& display, Lcdc& lcdc, MemoryRegisters& registers, ColourPalette& bgPalette, ColourPalette& oamPalette)
    : m_display     (display),
    m_lcdc          (lcdc),
    m_registers     (registers),
    m_bgPalette     (bgPalette),
    m_spritePalette (oamPalette),
    m_colour        (false),
    //m_fetcher(),
    m_droppedPixels (0),
    m_x             (0),
    m_window        (false),
    m_sprites       ({})
{
    //if (colour)
    //{
    //    //TODO create colour fifo
    //    assert(!colour); //not yet implemented!
    //}
    //else
    {
        m_fifo = std::make_unique<ClassicPixelFifo>(display, lcdc, registers);
    }
    m_fetcher = std::make_unique<Fetcher>(vram0, vram1, oam, lcdc, registers);
}

//public
void PixelTransferPhase::start(std::array<SpritePosition, 10> sprites)
{
    m_sprites = sprites;
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

        if (m_droppedPixels < m_registers.getByte(MemoryRegisters::SCX) % 0x08)
        {
            m_fifo->dropPixel();
            m_droppedPixels++;
            return true;
        }

        if (!m_window && m_lcdc.isWindowDisplay()
            && m_registers.getByte(MemoryRegisters::LY) >= m_registers.getByte(MemoryRegisters::WY)
            && m_x == (m_registers.getByte(MemoryRegisters::WX) - 7))
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
            for (auto i = 0u; i < m_sprites.size(); ++i)
            {
                auto& sprite = m_sprites[i];
                if (sprite.getAddress() == 0)
                {
                    continue;
                }

                if (m_x == 0 && sprite.getX() < 8)
                {
                    if (!spriteAdded)
                    {
                        m_fetcher->addSprite(sprite, 8 - sprite.getX(), static_cast<std::uint16_t>(i));
                        spriteAdded = true;
                    }
                    m_sprites[i] = {};
                }
                else if (sprite.getX() - 8 == m_x)
                {
                    if (!spriteAdded)
                    {
                        m_fetcher->addSprite(sprite, 0, static_cast<std::uint16_t>(i));
                        spriteAdded = true;
                    }
                    m_sprites[i] = {};
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

void PixelTransferPhase::enableColour(bool enable)
{
    if (enable)
    {
        m_fifo = std::make_unique<ColourPixelFifo>(m_lcdc, m_display, m_bgPalette, m_spritePalette);
    }
    else
    {
        m_fifo = std::make_unique<ClassicPixelFifo>(m_display, m_lcdc, m_registers);
    }
    m_colour = enable;
    m_fetcher->enableColour(enable, m_fifo.get());
}

//private
void PixelTransferPhase::startFetchingBackground()
{
    std::uint8_t bgX = m_registers.getByte(MemoryRegisters::SCX) / 0x08;
    std::uint8_t bgY = (m_registers.getByte(MemoryRegisters::SCY) + m_registers.getByte(MemoryRegisters::LY)) % 0x100;
    m_fetcher->startFetching(m_lcdc.getBgTileMapDisplay() + (bgY / 0x08) * 0x20, m_lcdc.getBgWindowTileData(), bgX, m_lcdc.isBgWindowTileDataSigned(), bgY % 0x08);
}

void PixelTransferPhase::startFetchingWindow()
{
    std::uint8_t winX = (m_x - m_registers.getByte(MemoryRegisters::WX) + 7) / 0x08;
    std::uint8_t winY = m_registers.getByte(MemoryRegisters::LY) - m_registers.getByte(MemoryRegisters::WY);
    m_fetcher->startFetching(m_lcdc.getWindowTileMapDisplay() + (winY / 0x08) * 0x20, m_lcdc.getBgWindowTileData(), winX, m_lcdc.isBgWindowTileDataSigned(), winY % 0x08);
}