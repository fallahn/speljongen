#include "ColourPalette.hpp"

ColourPalette::ColourPalette(std::uint16_t offset)
    : m_indexAddress    (offset),
    m_dataAddress       (offset + 1),
    m_index             (0),
    m_autoIncrement     (false)
{

}

//public
bool ColourPalette::accepts(std::uint16_t address) const
{
    return address == m_indexAddress || address == m_dataAddress;
}

void ColourPalette::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    if (address == m_indexAddress)
    {
        m_index = value & 0x3f;
        m_autoIncrement = (value & (1 << 7)) != 0;
    }
    else if (address == m_dataAddress)
    {
        auto colour = m_palettes[m_index / 8][(m_index % 8) / 2];
        if (m_index % 2 == 0)
        {
            colour = (colour & 0xff00) | value;
        }
        else
        {
            colour = (colour & 0x00ff) | (value << 8);
        }

        m_palettes[m_index / 8][(m_index % 8) / 2] = colour;
        if (m_autoIncrement)
        {
            m_index = (m_index + 1) & 0x3f;
        }
    }
}

std::uint8_t ColourPalette::getByte(std::uint16_t address) const
{
    assert(accepts(address));

    if (address == m_indexAddress)
    {
        return m_index | (m_autoIncrement ? 0x80 : 0) | 0x40;
    }

    auto colour = m_palettes[m_index / 8][(m_index % 8) / 2];
    if (m_index % 2 == 0)
    {
        return colour & 0xff;
    }
    
    return (colour >> 8) & 0xff;
}

const Palette& ColourPalette::getPalette(std::size_t index) const
{
    return m_palettes[index];
}

void ColourPalette::clear()
{
    for (auto& palette : m_palettes)
    {
        for (auto& p : palette)
        {
            p = 0x7fff;
        }
    }
}