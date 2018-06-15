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
#include <vector>
#include <cassert>
#include <string>
#include <iostream>

class AddressSpace
{
public:

    explicit AddressSpace(std::vector<std::uint8_t>& storage) :m_storage(storage) {}

    virtual ~AddressSpace() = default;

    virtual std::string getLabel() const { return "Label Missing"; }

    virtual bool accepts(std::uint16_t address) const = 0;

    virtual void setByte(std::uint16_t address, std::uint8_t value) = 0;

    virtual std::uint8_t getByte(std::uint16_t address) const = 0;

protected:
    std::vector<std::uint8_t>& getStorage()
    {
        return m_storage;
    }

    const std::vector<std::uint8_t>& getStorage() const
    {
        return m_storage;
    }

    std::uint8_t getStorageValue(std::uint16_t address) const { return m_storage[address]; }

    void setStorageValue(std::uint16_t address, std::uint8_t value) 
    {
        m_storage[address] = value; 
    }
private:
    std::vector<std::uint8_t>& m_storage;
};