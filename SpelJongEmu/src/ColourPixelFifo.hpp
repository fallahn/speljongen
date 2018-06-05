#pragma once

#include "PixelFifo.hpp"

class Lcdc;
class Display;
class ColourPalette;
class ColourPixelFifo final : public PixelFifo
{
public:
    ColourPixelFifo(Lcdc&, Display&, ColourPalette&, ColourPalette&);

    std::size_t length() const override;
    void blitPixel() override;
    void dropPixel() override;
    void enqueue8Pixels(const std::array<std::uint8_t, 8>& pixels, const TileAttributes&) override;
    void setOverlay(const std::array<std::uint8_t, 8>& pixelLine, std::uint16_t offset, const TileAttributes& flags, std::uint16_t oamIndex) override;
    void clear() override;

private:
    PixelQueue m_pixels;
    PixelQueue m_palettes;
    IndexedQueue<std::int8_t, 16u> m_priorities;

    Lcdc& m_lcdc;
    Display& m_display;
    ColourPalette& m_backgroundPalette;
    ColourPalette& m_spritePalette;

    std::uint16_t dequeuePixel();
    std::uint16_t getColour(std::int8_t priority, std::uint8_t palette, std::uint8_t colour);
};