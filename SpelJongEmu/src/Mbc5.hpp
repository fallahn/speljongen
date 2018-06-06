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

class Mbc5 final : public AddressSpace
{
public:
    Mbc5(std::vector<std::uint8_t>&, const std::vector<char>&, std::int32_t, std::int32_t);

    std::string getLabel() const override { return "MBC 5"; }

    bool accepts(std::uint16_t) const override;

    void setByte(std::uint16_t, std::uint8_t) override;

    std::uint8_t getByte(std::uint16_t) const override;

private:
    std::vector<std::uint8_t> m_cartridgeData;
    std::vector<std::uint8_t> m_ram;
    std::int32_t m_romBanks;
    std::int32_t m_ramBanks;

    std::int32_t m_selectedRomBank;
    std::int32_t m_selectedRamBank;

    bool m_ramWriteEnabled;

    std::uint8_t getRomByte(std::int32_t bank, std::uint16_t address) const;
    std::uint16_t getRamAddress(std::uint16_t address) const;
};