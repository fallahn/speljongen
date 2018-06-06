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

#include "MemoryRegisters.hpp"

#include <array>

class TileAttributes final
{
public:
    static TileAttributes valueOf(std::uint8_t value)
    {
        static std::array<TileAttributes, 256> attributes = {};
        if (attributes[255].m_value == 0)
        {
            for (auto i = 0; i < 256; ++i) attributes[i] = TileAttributes(i);
        }
        return attributes[value];
    }

    TileAttributes(std::uint8_t value = 0) : m_value(value) {}

    bool isPriority() const { return (m_value & (1 << 7)) != 0; }

    bool isYFlip() const { return (m_value & (1 << 6)) != 0; }

    bool isXFlip() const { return (m_value & (1 << 5)) != 0; }

    std::uint16_t getClassicPalette() const { return (m_value & (1 << 4)) == 0 ? MemoryRegisters::OBP0 : MemoryRegisters::OBP1; }

    std::uint8_t getBank() const { return (m_value & (1 << 3)) == 0 ? 0 : 1; }

    std::uint8_t getColourPaletteIndex() const { return m_value & 0x07; }

private:
    std::uint8_t m_value;
};