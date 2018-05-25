#include "ColourPalette.hpp"

ColourPalette::ColourPalette(std::vector<std::uint8_t>& storage, std::uint16_t offset)
    : AddressSpace      (storage),
    m_indexAddress      (offset),
    m_dataAddress       (offset + 1),
    //m_index             (0),
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
        setStorageValue(address, value & 0x3f);
        m_autoIncrement = (value & (1 << 7)) != 0;
    }
    else if (address == m_dataAddress)
    {
        auto currValue = getStorageValue(m_indexAddress);
        auto colour = m_palettes[currValue / 8][(currValue % 8) / 2];
        if (currValue % 2 == 0)
        {
            colour = (colour & 0xff00) | value;
        }
        else
        {
            colour = (colour & 0x00ff) | (value << 8);
        }

        m_palettes[currValue / 8][(currValue % 8) / 2] = colour;
        if (m_autoIncrement)
        {
            setStorageValue(m_indexAddress, (currValue + 1) & 0x3f);
        }
    }
}

std::uint8_t ColourPalette::getByte(std::uint16_t address) const
{
    assert(accepts(address));

    if (address == m_indexAddress)
    {
        return getStorageValue(address) | (m_autoIncrement ? 0x80 : 0) | 0x40;
    }

    auto colour = m_palettes[getStorageValue(address) / 8][(getStorageValue(address) % 8) / 2];
    if (getStorageValue(address) % 2 == 0)
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