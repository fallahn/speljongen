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

#include <array>
#include <cstdint>

namespace BitUtil
{
    static inline std::uint8_t getMSB(std::uint16_t word)
    {
        return word >> 8;
    }

    static inline std::uint8_t getLSB(std::uint16_t word)
    {
        return word & 0xff;
    }

    static inline std::uint16_t toWord(std::uint8_t msb, std::uint8_t lsb)
    {
        return (msb << 8) | lsb;
    }

    static inline std::uint16_t toWord(std::array<std::uint8_t, 2u> bytes)
    {
        return toWord(bytes[1], bytes[0]);
    }

    static inline bool getBit(std::uint8_t byte, std::size_t position)
    {
        return (byte & (1 << position)) != 0;
    }

    static inline std::uint8_t clearBit(std::uint8_t byte, std::size_t position)
    {
        return byte & ~(1 << position);
    }

    static inline std::uint8_t setBit(std::uint8_t byte, std::size_t position)
    {
        return byte | (1 << position);
    }

    static inline std::int8_t toSigned(std::uint8_t byte)
    {
        return static_cast<std::uint8_t>(byte);
    }
}