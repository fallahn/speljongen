#pragma once

#include "GpuRegister.hpp"

#include <array>

class TileAttributes final
{
public:
    static TileAttributes valueOf(std::uint8_t value)
    {
        static std::array<TileAttributes, 256> attributes = {};
        if (attributes[255].m_value == 0)
        {
            for (auto i = 0; i < 256; ++i) attributes[i] = TileAttributes(i);
        }
        return attributes[value];
    }

    explicit TileAttributes(std::uint8_t value = 0) : m_value(value) {}

    bool isPriority() const { return (m_value & (1 << 7)) != 0; }

    bool isYFlip() const { return (m_value & (1 << 6)) != 0; }

    bool isXFlip() const { return (m_value & (1 << 5)) != 0; }

    GpuRegister getClassicPalette() const { return (m_value & (1 << 4)) == 0 ? GpuRegister::registers[GpuRegister::OBP0] : GpuRegister::registers[GpuRegister::OBP1]; }

    std::uint8_t getBank() const { return (m_value & (1 << 3)) == 0 ? 0 : 1; }

    std::uint8_t getColourPaletteIndex() const { return m_value & 0x07; }

private:
    std::uint8_t m_value;
};