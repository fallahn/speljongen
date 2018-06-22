#include "MBC2.hpp"
#include "BatterySaves.hpp"

Mbc2::Mbc2(std::vector<std::uint8_t>& storage, const std::vector<char>& cart, std::int32_t romCount, std::int32_t& selectedRom)
    : AddressSpace      (storage),
    m_romBanks          (romCount),
    m_selectedRomBank   (selectedRom)
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
bool Mbc2::accepts(std::uint16_t address) const
{
    return address < 0x8000 || (address >= 0xa000 && address < 0xc000);
}

void Mbc2::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));

    if (address < 0x2000)
    {
        if ((address & 0x0100) == 0)
        {
            m_ramWriteEnabled = (value & 0xa) != 0;
            if (!m_ramWriteEnabled)
            {
                Battery::save(m_ram);
            }
        }
    }
    else if (address >= 0x2000 && address < 0x4000)
    {
        if ((address & 0x0100) != 0)
        {
            m_selectedRomBank = value & 0xf;
        }
    }
    else if (address >= 0xa000 && address < 0xc000 && m_ramWriteEnabled)
    {
        auto ramAddress = getRamAddress(address);
        assert(ramAddress < m_ram.size());
        m_ram[ramAddress] = value;
    }
}

std::uint8_t Mbc2::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    if (address < 0x2000)
    {
        return getRomByte(0, address);
    }

    if (address >= 0x4000 && address < 0x8000)
    {
        return getRomByte(m_selectedRomBank, address - 0x4000);
    }

    if (address >= 0xa000 && address < 0xc000)
    {
        auto ramAddress = getRamAddress(address);
        assert(ramAddress < m_ram.size());

        return m_ram[ramAddress];
    }
    return 0xff;
}

//private
std::uint8_t Mbc2::getRomByte(std::int32_t romBank, std::uint16_t address) const
{
    std::size_t offset = romBank * 0x4000 + address;
    assert(offset < m_cartridgeData.size());
    return m_cartridgeData[offset];
}

std::uint16_t Mbc2::getRamAddress(std::uint16_t address) const
{
    return address - 0xa000;
}