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
#include "SpritePosition.hpp"
#include <array>

class Ram;
class Lcdc;
class GpuRegister;
class OamSearchPhase final : public GpuPhase
{
public:
    OamSearchPhase(Ram& oamRam, Lcdc& lcdc, MemoryRegisters& registers);

    void start();
    bool tick() override;

    std::array<SpritePosition, 10> getSprites() const { return m_sprites; }
    
private:
    enum State
    {
        ReadingX, ReadingY
    }m_state;

    Ram& m_oamRam;
    Lcdc& m_lcdc;
    MemoryRegisters&  m_registers;
    std::array<SpritePosition, 10> m_sprites;
    std::uint8_t m_spritePositionX;
    std::uint8_t m_spritePositionY;
    std::uint16_t m_index;
    std::size_t m_spritePosIndex;

    bool between(std::uint8_t from, std::uint8_t x, std::uint8_t to);
};