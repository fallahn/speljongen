#pragma once

#include "AddressSpace.hpp"

#include <array>

using Palette = std::array<std::uint16_t, 4u>;
class ColourPalette final : public AddressSpace 
{
public:
    explicit ColourPalette(std::uint16_t offset);

    bool accepts(const std::uint16_t address) const override;

    void setByte(std::uint16_t, std::uint8_t) override;

    std::uint8_t getByte(std::uint16_t) const override;

    const Palette& getPalette(std::size_t) const;

    void clear();

private:
    std::array<Palette, 8u>  m_palettes;
    std::uint16_t m_indexAddress;
    std::uint16_t m_dataAddress;
    std::uint8_t m_index;
    bool m_autoIncrement;
};
