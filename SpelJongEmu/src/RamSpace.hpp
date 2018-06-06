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
/*
Differs from RAM class by echoing the bytes written to
the shadow space address
*/

#include "AddressSpace.hpp"

class RamSpace final : public AddressSpace
{
public:
    RamSpace(std::vector<std::uint8_t>& storage, std::uint16_t start, std::uint16_t size, std::uint16_t echoOffset)
        : AddressSpace(storage),
        m_startAddress(start), m_endAddress(start + size), m_echoOffset(echoOffset)
    {
        assert(m_startAddress < m_endAddress);
    }

    std::string getLabel() const override { return "Shadowed RAM"; }

    bool accepts(std::uint16_t address) const override
    {
        return (address >= m_startAddress && address < m_endAddress);
    }

    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        assert(accepts(address));
        setStorageValue(address, value);
        setStorageValue(address + m_echoOffset, value);
    }

    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        return getStorageValue(address);
    }

private:
    std::uint16_t m_startAddress;
    std::uint16_t m_endAddress;
    std::uint16_t m_echoOffset;
};