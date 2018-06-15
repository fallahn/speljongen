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

#include <cassert>

class Lcdc final : public AddressSpace
{
public:

    explicit Lcdc(std::vector<std::uint8_t>& storage) : AddressSpace(storage) { set(0x91); }

    std::string getLabel() const override { return "LCDC"; }

    bool isBgAndWindowDisplay() const { return (get() & 0x01) != 0; }
    bool isObjDisplay() const { return (get() & 0x02) != 0; }
    std::uint8_t getSpriteHeight() const { return ((get() & 0x04) == 0) ? 8 : 16; }
    std::uint16_t getBgTileMapDisplay() const { return ((get() & 0x08) == 0) ? 0x9800 : 0x9c00; }
    //although the other set actually starts at 0x8800 the negative offset is applied by the fetcher
    std::uint16_t getBgWindowTileData() const { return ((get() & 0x10) == 0) ? 0x9000 : 0x8000; }
    bool isBgWindowTileDataSigned() const { return (get() & 0x10) == 0; }
    bool isWindowDisplay() const { return (get() & 0x20) != 0; }
    std::uint16_t getWindowTileMapDisplay() const { return ((get() & 0x40) == 0) ? 0x9800 : 0x9c00; }
    bool isLcdEnabled() const { return (get() & 0x80) != 0; }

    bool accepts(std::uint16_t address) const override { return address == 0xff40; }
    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        assert(accepts(address));
        setStorageValue(address, value);
    }
    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        return getStorageValue(address);
    }
    void set(std::uint8_t value) { setStorageValue(0xff40, value); }
    std::uint32_t get() const { return getStorageValue(0xff40); }

private:

};