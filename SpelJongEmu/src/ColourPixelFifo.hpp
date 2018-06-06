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