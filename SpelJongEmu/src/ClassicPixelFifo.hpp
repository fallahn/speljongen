/*
MIT License

Copyright(c) 2017 Tomasz R?kawek(Coffee GB)
Copyright(c) 2018 Matt Marchant(Speljongen)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
