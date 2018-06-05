#pragma once

#include "GpuPhase.hpp"
#include "MemoryRegisters.hpp"
#include "PixelFifo.hpp"
#include "Fetcher.hpp"
#include "SpritePosition.hpp"

#include <memory>
#include <array>

class Ram;
class Display;
class Lcdc;
class ColourPalette;

class PixelTransferPhase final : public GpuPhase
{
public:
    PixelTransferPhase(Ram&, Ram&, Ram&, Display&, Lcdc&, MemoryRegisters&, ColourPalette&, ColourPalette&);

    void start(std::array<SpritePosition, 10> sprites);

    bool tick() override;

    void enableColour(bool);

private:
    Display & m_display;
    Lcdc& m_lcdc;
    MemoryRegisters& m_registers;
    ColourPalette& m_bgPalette;
    ColourPalette& m_spritePalette;
    bool m_colour;

    std::uint16_t m_droppedPixels;
    std::uint8_t m_x;
    bool m_window;

    std::array<SpritePosition, 10u> m_sprites;

    std::unique_ptr<PixelFifo> m_fifo;
    std::unique_ptr<Fetcher> m_fetcher; //baaaaaaahhhh

    void startFetchingBackground();
    void startFetchingWindow();
};