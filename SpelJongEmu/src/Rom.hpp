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

class Rom final : public AddressSpace
{
public:
    explicit Rom(std::vector<std::uint8_t>& storage, const std::vector<char>& buf)
        : AddressSpace(storage)
    {
        for (auto i = 0u; i < buf.size() && i < 0x8000; ++i)
        {
            storage[i] = buf[i];
        }
    }

    std::string getLabel() const override { return "ROM"; }

    bool accepts(std::uint16_t address) const override
    {
        return (address < 0x8000) ||
            (address >= 0xa000 && address < 0xc000);
    }

    void setByte(std::uint16_t, std::uint8_t) override
    {
        //assert(false);
    }

    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        return getStorageValue(address);
    }
};
