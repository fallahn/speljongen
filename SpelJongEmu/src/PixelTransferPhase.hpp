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
    Ram& m_vram0;
    Ram& m_vram1;
    Ram& m_oam;
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