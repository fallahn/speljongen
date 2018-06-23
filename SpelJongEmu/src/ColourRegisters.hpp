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

class ColourRegisters final : public AddressSpace
{
public:
    explicit ColourRegisters(std::vector<std::uint8_t>& storage)
        : AddressSpace(storage)
    {
        storage[0xff6c] = 0xfe;
        storage[0xff74] = 0xff;
        storage[0xff75] = 0x8f;
    }

    void reset()
    {
        setByte(0xff6c, 0);
        setByte(0xff74, 0xff);
        setByte(0xff75, 0);
    }

    std::string getLabel() const override { return "Colour Registers"; }

    bool accepts(std::uint16_t address) const override
    {
        return address == 0xff6c || (address >= 0xff72 && address < 0xff78);
    }

    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        assert(accepts(address));
        switch (address)
        {
        case 0xff6c:
            setStorageValue(address, 0xfe | (value & 0x01));
            break;
        case 0xff75:
            setStorageValue(address, 0x8f | (value & 0x70));
            break;
        default:
            setStorageValue(address, value);
            break;
        }
    }

    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        return getStorageValue(address);
    }
};