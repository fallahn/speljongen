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

//creates an address space used as RAM.

#include "AddressSpace.hpp"

#include <cassert>
#include <iostream>

class Ram final : public AddressSpace
{
public:
    Ram(std::vector<std::uint8_t>& storage, std::uint16_t start, std::uint16_t length, bool sharedStorage = true)
        : AddressSpace  (storage),
        m_start         (start),
        m_end           (start + length),
        m_useInternal   (false)
    {
        assert(m_end > m_start); //values have wrpapped around by becoming negative or something :S
        if (!sharedStorage)
        {
            //set storage to local
            m_ownStorage.resize(m_end); //it's a bit of a waste, but meh
            m_useInternal = true;
        }
    }

    std::string getLabel() const override { return "RAM"; }

    bool accepts(std::uint16_t address) const override
    {
        return (address >= m_start && address < m_end);
    }

    void setByte(std::uint16_t address, std::uint8_t value) override
    {       
        assert(accepts(address));
        if (m_useInternal)
        {
            m_ownStorage[address] = value;
            return;
        }
        getStorage()[address] = value;
    }

    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        if (m_useInternal)
        {
            return m_ownStorage[address];
        }
        return getStorage()[address];
    }

    void clear()
    {
        if (m_useInternal)
        {
            for (auto& c : m_ownStorage) c = 0;
        }
    }

private:
    std::uint16_t m_start = 0;
    std::uint16_t m_end = 0;
    std::vector<std::uint8_t> m_ownStorage;
    bool m_useInternal;
};