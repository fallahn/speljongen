#pragma once

#include <cstdint>

struct SpritePosition final
{
    SpritePosition(std::uint16_t x = 0, std::uint16_t y = 0, std::uint16_t address = 0)
        : m_x(x), m_y(y), m_address(address) {}

    std::uint16_t getX() const { return m_x; }
    std::uint16_t getY() const { return m_y; }
    std::uint16_t getAddress() const { return m_address; }

private:
    std::uint16_t m_x;
    std::uint16_t m_y;
    std::uint16_t m_address;
};
