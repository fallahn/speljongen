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

/*
The colour gameboy can map one of 7 banks into the 0xd000 - 0xdfff range.
The selected bank is stored in the SVBK (switch video bank) register at 0xff70
*/

#pragma once

#include "AddressSpace.hpp"

class ColourRam final : public AddressSpace
{
public:
    explicit ColourRam(std::vector<std::uint8_t>& storage)
        : AddressSpace(storage), m_bankData(7 * 0x1000) {}

    std::string getLabel() const override { return "Colour RAM"; }

    bool accepts(std::uint16_t address) const override
    {
        return address == 0xff70 || (address >= 0xd000 && address < 0xe000);
    }

    void setByte(std::uint16_t address, std::uint8_t value)
    {
        assert(accepts(address));
        if (address == 0xff70)
        {
            setStorageValue(address, value);
        }
        else
        {
            //look up internal bank
            m_bankData[getBankAddress(address)] = value;
        }
    }

    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        if (address == 0xff70)
        {
            return getStorageValue(address);
        }

        return m_bankData[getBankAddress(address)];
    }

private:
    std::vector<std::uint8_t> m_bankData;

    std::int32_t getBankAddress(std::uint16_t address) const
    {
        std::int32_t bankAddress = getStorageValue(0xff70) & 0x7;
        bankAddress = std::max(1, bankAddress); //if svbk is 0 we still get bank 1

        bankAddress = (bankAddress - 1) * 0x1000;
        bankAddress += (address - 0xd000);

        return bankAddress;
    }
};