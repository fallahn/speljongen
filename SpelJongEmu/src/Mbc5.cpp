#include "Mbc5.hpp"

#include <algorithm>
#include <iostream>

Mbc5::Mbc5(std::vector<std::uint8_t>& storage, const std::vector<char>& cartData, std::int32_t romBanks, std::int32_t ramBanks)
    : AddressSpace      (storage),
    m_ram               (0x2000 * std::max(1, ramBanks)),
    m_romBanks          (romBanks),
    m_ramBanks          (ramBanks),
    m_selectedRomBank   (1),
    m_selectedRamBank   (0),
    m_ramWriteEnabled   (false)
{
    for (auto c : cartData) 
    {
        m_cartridgeData.push_back(c);
    }

    for (auto& c : m_ram)
    {
        c = 0xff;
    }
}

//public
bool Mbc5::accepts(std::uint16_t address) const
{
    return address < 0x8000 || (address >= 0xa000 && address < 0xc000);
}

void Mbc5::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    if (address < 0x2000)
    {
        m_ramWriteEnabled = (value & 0b1010) != 0;
        //TODO save ram when battery implemented

        return;
    }

    if (address >= 0x2000 && address < 0x3000)
    {
        m_selectedRomBank = (m_selectedRomBank & 0x100) | value;
        //std::cout << "ROM bank set to: " << m_selectedRomBank << "\n";
        return;
    }

    if (address >= 0x3000 && address < 0x4000)
    {
        std::int32_t val = value;
        m_selectedRomBank = (m_selectedRomBank & 0x0ff) | ((val & 1) << 8);
        //std::cout << "ROM bank set to: " << m_selectedRomBank << "\n";
        return;
    }

    if (address >= 0x4000 && address < 0x6000)
    {
        auto bank = value & 0x0f;
        if (bank < m_ramBanks)
        {
            m_selectedRamBank = bank;
        }
        return;
    }

    if (address >= 0xa000 && address < 0xc000 && m_ramWriteEnabled)
    {
        std::uint16_t ramAddress = getRamAddress(address);
        assert(ramAddress < m_ram.size());
        m_ram[ramAddress] = value;
    }
}

std::uint8_t Mbc5::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    if (address < 0x4000)
    {
        return getRomByte(0, address);
    }

    if (address >= 0x4000 && address < 0x8000)
    {
        return getRomByte(m_selectedRomBank, address - 0x4000);
    }

    if (address >= 0xa000 && address < 0xc000)
    {
        std::uint16_t ramAddress = getRamAddress(address);
        assert(ramAddress < m_ram.size());
        return m_ram[ramAddress];
    }

    return 0xff;
}

//private
std::uint8_t Mbc5::getRomByte(std::int32_t bank, std::uint16_t address) const
{
    std::uint16_t offset = bank * 0x4000 + address;
    assert(offset < m_cartridgeData.size());
    return m_cartridgeData[offset];
}

std::uint16_t Mbc5::getRamAddress(std::uint16_t address) const
{
    return m_selectedRamBank * 0x2000 + (address - 0xa000);
}