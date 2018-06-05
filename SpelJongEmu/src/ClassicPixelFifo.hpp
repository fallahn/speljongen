#pragma once

/*
Implements the pixel queue for the original gameboy
*/

#include "PixelFifo.hpp"
#include "MemoryRegisters.hpp"

class Display;
class Lcdc;
class ClassicPixelFifo final : public PixelFifo
{
public:
    ClassicPixelFifo(Display&, Lcdc&, MemoryRegisters&);

    std::size_t length() const override;

    void blitPixel() override;

    void dropPixel() override;

    std::uint8_t dequeuePixel();

    void enqueue8Pixels(const std::array<std::uint8_t, 8u>&, const TileAttributes&) override;

    void setOverlay(const std::array<std::uint8_t, 8u>&, std::uint16_t, const TileAttributes&, std::uint16_t) override;

    const PixelQueue& getPixels() const;

    void clear() override;

private:

    PixelQueue m_pixels;
    PixelQueue m_palettes;
    PixelQueue m_pixelType; //0 = bg, 1 = sprite/obj
    Display& m_display;
    Lcdc& m_lcdc;
    MemoryRegisters& m_memoryRegisters;

    std::uint8_t getColour(std::uint8_t palette, std::uint8_t index) const;
};
