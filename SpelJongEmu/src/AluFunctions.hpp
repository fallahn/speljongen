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

#include <cstdint>

class Flags;
namespace AluFunction
{
    std::uint8_t inc(Flags&, std::uint8_t);
    std::uint16_t inc16(Flags&, std::uint16_t);
    std::uint8_t dec(Flags&, std::uint8_t);
    std::uint16_t dec16(Flags&, std::uint16_t);
    std::uint16_t add(Flags&, std::uint16_t, std::uint16_t);
    std::uint16_t add(Flags&, std::uint16_t, std::int8_t);
    std::uint16_t add_sp(Flags&, std::uint16_t, std::int8_t);
    std::uint8_t daa(Flags&, std::uint8_t);
    std::uint8_t cpl(Flags&, std::uint8_t);
    std::uint8_t scf(Flags&, std::uint8_t);
    std::uint8_t ccf(Flags&, std::uint8_t);
    std::uint8_t add(Flags&, std::uint8_t, std::uint8_t);
    std::uint8_t adc(Flags&, std::uint8_t, std::uint8_t);
    std::uint8_t sub(Flags&, std::uint8_t, std::uint8_t);
    std::uint8_t sbc(Flags&, std::uint8_t, std::uint8_t);
    std::uint8_t AND(Flags&, std::uint8_t, std::uint8_t);
    std::uint8_t OR(Flags&, std::uint8_t, std::uint8_t);
    std::uint8_t XOR(Flags&, std::uint8_t, std::uint8_t);
    std::uint8_t cp(Flags&, std::uint8_t, std::uint8_t);
    std::uint8_t rlc(Flags&, std::uint8_t);
    std::uint8_t rrc(Flags&, std::uint8_t);
    std::uint8_t rl(Flags&, std::uint8_t);
    std::uint8_t rr(Flags&, std::uint8_t);
    std::uint8_t sla(Flags&, std::uint8_t);
    std::uint8_t sra(Flags&, std::uint8_t);
    std::uint8_t swap(Flags&, std::uint8_t);
    std::uint8_t srl(Flags&, std::uint8_t);
    std::uint8_t bit(Flags&, std::uint8_t, std::uint8_t);
    std::uint8_t res(std::uint8_t, std::uint8_t);
    std::uint8_t set(std::uint8_t, std::uint8_t);
};
