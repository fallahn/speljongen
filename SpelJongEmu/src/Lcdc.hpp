#pragma once

#include "AddressSpace.hpp"

#include <cassert>

class Lcdc final : public AddressSpace
{
public:

    bool isBgAndWindowDisplay() const { return (m_value & 0x01) != 0; }
    bool isObjDisplay() const { return (m_value & 0x02) != 0; }
    std::uint8_t getSpriteHeight() const { return ((m_value & 0x04) == 0) ? 8 : 16; }
    std::uint16_t getBgTileMapDisplay() const { return ((m_value & 0x08) == 0) ? 0x9800 : 0x9c00; }
    std::uint16_t getBgWindowTileData() const { return ((m_value & 0x10) == 0) ? 0x9000 : 0x8000; }
    bool isBgWindowTileDataSigned() const { return (m_value & 0x10) == 0; }
    bool isWindowDisplay() const { return (m_value & 0x20) != 0; }
    std::uint16_t getWindowTileMapDisplay() const { return ((m_value & 0x40) == 0) ? 0x9800 : 0x9c00; }
    bool isLcdEnabled() const { return (m_value & 0x80) != 0; }

    bool accepts(std::uint16_t address) const override { return address == 0xff40; }
    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        assert(accepts(address));
        m_value = value;
    }
    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        return m_value;
    }
    void set(std::uint8_t value) { m_value = value; }
    std::uint32_t get() const { return m_value; }

private:
    std::uint8_t m_value = 0x91;
};