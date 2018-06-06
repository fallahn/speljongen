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

#include "AddressSpace.hpp"
#include <cstdint>


class MemoryRegisters final : public AddressSpace
{
public:
    MemoryRegisters(std::vector<std::uint8_t>& storage, std::uint16_t start, std::uint16_t end);
    
    std::string getLabel() const { return "Memory Registers"; }

    bool accepts(std::uint16_t address) const override;

    void setByte(std::uint16_t address, uint8_t value) override;

    std::uint8_t getByte(std::uint16_t address) const override;

    std::uint8_t preIncrement(std::uint16_t);

    void reset();

    enum
    {
        P1 = 0xff00,
        SB = 0xff01,
        SC = 0xff02,
        DIV = 0xff04,
        TIMA = 0xff05,
        TMA = 0xff06,
        TAC = 0xff07,
        IF = 0xff0f,
        NR10 = 0xff10,
        NR11 = 0xff11,
        NR12 = 0xff12,
        NR13 = 0xff13,
        NR14 = 0xff14,
        NR21 = 0xff16,
        NR22 = 0xff17,
        NR23 = 0xff18,
        NR24 = 0xff19,
        NR30 = 0xff1a,
        NR31 = 0xff1b,
        NR32 = 0xff1c,
        NR33 = 0xff1d,
        NR34 = 0xff1e,
        NR41 = 0xff20,
        NR42 = 0xff21,
        NR43 = 0xff22,
        NR44 = 0xff23,
        NR50 = 0xff24,
        NR51 = 0xff25,
        NR52 = 0xff26,
        LCDC = 0xff40,
        STAT = 0xff41,
        SCY = 0xff42,
        SCX = 0xff43,
        LY = 0xff44,
        LYC = 0xff45,
        DMA = 0xff46,
        BGP = 0xff47,
        OBP0 = 0xff48,
        OBP1 = 0xff49,
        WY = 0xff4a,
        WX = 0xff4b,
        VBK = 0xff4f
    };

private:

    enum ReadFlags
    {
        Read = 0b01,
        Write = 0b10,
        ReadWrite = 0b11
    };

    std::vector<ReadFlags> m_readFlags;
    std::uint16_t m_start;
    std::uint16_t m_end;
};