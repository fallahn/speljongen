#include "Mbc3.hpp"
#include "BatterySaves.hpp"

#include <algorithm>

Mbc3::Mbc3(std::vector<std::uint8_t>& storage, const std::vector<char>& cart, std::int32_t ramBanks, std::int32_t& selectedRom)
    : AddressSpace      (storage),
    m_ram               (0x2000 * std::max(1, ramBanks)),
    m_ramBanks          (ramBanks),
    m_selectedRomBank   (selectedRom),
    m_selectedRamBank   (0),
    m_ramWriteEnabled   (false)
{
    for (auto c : cart)
    {
        m_cartridgeData.push_back(c);
    }

    for (auto& c : m_ram)
    {
        c = 0xff;
    }

    Battery::load(m_ram);
}

//public
bool Mbc3::accepts(std::uint16_t address) const
{
    return address < 0x8000 || (address >= 0xa000 && address < 0xc000);
}

void Mbc3::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    if (address < 0x2000)
    {
        m_ramWriteEnabled = (value & 0b1010) != 0;
        if (!m_ramWriteEnabled)
        {
            Battery::save(m_ram);
            return;
        }

        if (address >= 0x2000 && address < 0x4000)
        {
            std::int32_t bank = value & 0x7f;
            selectRomBank(bank);
            return;
        }

        if (address >= 0x4000 && address < 0x6000)
        {
            assert(value <= m_ramBanks);
            m_selectedRamBank = value;
            return;
        }

        if (address >= 0x6000 && address < 0x8000)
        {
            //TODO implement RTC
            return;
        }

        if (address >= 0xa000 && address < 0xc000
            && m_ramWriteEnabled
            && m_selectedRamBank < 1)
        {
            std::uint16_t ramAddress = getRamAddress(address);
            assert(ramAddress < m_ram.size());
            m_ram[ramAddress] = value;
            return;
        }

        if (address >= 0xa000 && address < 0xc000
            && m_ramWriteEnabled
            && m_selectedRamBank > 3)
        {
            //TODO write to RTC
            return;
        }
    }
}

std::uint8_t Mbc3::getByte(std::uint16_t address) const
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

    if (address >= 0x8000 && address < 0xc000)
    {
        if (m_selectedRamBank < 4)
        {
            auto ramAddress = getRamAddress(address);
            assert(ramAddress < m_ram.size());
            return m_ram[ramAddress];
        }
        else
        {
            //TODO get RTC value
            return 0xff;
        }
    }
    return 0xff;
}

//private
void Mbc3::selectRomBank(std::int32_t bank)
{
    if (bank == 0) bank = 1;
    m_selectedRomBank = bank;
}

std::uint8_t Mbc3::getRomByte(std::int32_t romBank, std::uint16_t address) const
{
    std::size_t offset = romBank * 0x4000 + address;
    assert(offset < m_cartridgeData.size());
    return m_cartridgeData[offset];
}

std::uint16_t Mbc3::getRamAddress(std::uint16_t address) const
{
    return m_selectedRamBank * 0x2000 + (address - 0xa000);
}