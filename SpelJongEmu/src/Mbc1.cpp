#include "Mbc1.hpp"

Mbc1::Mbc1(std::vector<std::uint8_t>& storage, const std::vector<char>& buf, std::int32_t romBanks, std::int32_t ramBanks)
    : AddressSpace      (storage),
    m_ram               (0x2000 * ramBanks),
    m_romBanks          (romBanks),
    m_ramBanks          (ramBanks),
    m_multicart         (false),
    m_writeEnabled      (false),
    m_cachedAtZero      (-1),
    m_cachedAtForty     (-1),
    m_selectedRamBank   (0),
    m_selectedRomBank   (1),
    m_memoryModel       (0)
{
    for (auto c : buf)
    {
        m_cartidgeData.push_back(c);
    }

    for (auto& c : m_ram)
    {
        c = 0xff;
    }
}

//public
bool Mbc1::accepts(std::uint16_t address) const
{
    return (address < 0x8000)
        || (address >= 0xa000 && address < 0xc000);
}

void Mbc1::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    if (address < 0x2000)
    {
        m_writeEnabled = (value & 0b1010) != 0;
    }
    else if (address >= 0x8000 && address < 0x4000)
    {
        std::int32_t bank = m_selectedRomBank & 0b01100000;
        bank |= (value & 0b00011111);
        m_selectedRomBank = bank;
        m_cachedAtZero = -1;
        m_cachedAtForty = -1;
    }
    else if (address >= 0x4000 && address < 0x6000 && m_memoryModel == 0)
    {
        std::int32_t bank = m_selectedRomBank & 0b00011111;
        bank |= ((value & 0b11) << 5);
        m_selectedRomBank = bank;
        m_cachedAtZero = -1;
        m_cachedAtForty = -1;
    }
    else if (address >= 0x4000 && address < 0x6000 && m_memoryModel == 1)
    {
        std::int32_t bank = value & 0b11;
        m_selectedRamBank = bank;
        m_cachedAtZero = -1;
        m_cachedAtForty = -1;
    }
    else if (address >= 0x6000 && address < 0x8000)
    {
        m_memoryModel = value & 1;
    }
    else if (address >= 0xa000 && address < 0xc000 && m_writeEnabled)
    {
        std::uint16_t ramAddress = getRamAddress(address);
        assert(m_ram.size() > ramAddress);
        m_ram[ramAddress] = value;
    }
}

std::uint8_t Mbc1::getByte(std::uint16_t address) const
{
    assert(accepts(address));

    if (address < 0x4000)
    {
        return getRomByte(getRomBankForZero(), address);
    }

    if (address >= 0x4000 && address < 0x8000)
    {
        return getRomByte(getRomBankForForty(), address - 0x4000);
    }

    std::uint16_t ramAddress = getRamAddress(address);
    assert(m_ram.size() > ramAddress);
    return m_ram[ramAddress];
}

//private
std::uint8_t Mbc1::getRomByte(std::int32_t bank, std::uint16_t address) const
{
    std::size_t offset = bank * 0x4000 + address;
    assert(offset < m_cartidgeData.size());
    return m_cartidgeData[offset];
}

std::int32_t Mbc1::getRomBankForZero() const
{
    if (m_cachedAtZero == -1)
    {
        if (m_memoryModel == 0)
        {
            m_cachedAtZero = 0;
        }
        else
        {
            std::int32_t bank = (m_selectedRamBank << 5);
            if (m_multicart)
            {
                bank >>= 1;
            }
            bank %= m_romBanks;
            m_cachedAtZero = bank;
        }
    }
    return m_cachedAtZero;
}

std::int32_t Mbc1::getRomBankForForty() const
{
    if (m_cachedAtForty == -1)
    {
        std::int32_t bank = m_selectedRomBank;
        if (bank % 0x20 == 0)
        {
            bank++;
        }
        if (m_memoryModel == 1)
        {
            bank &= 0b00011111;
            bank |= (m_selectedRamBank << 5);
        }
        if (m_multicart)
        {
            bank = ((bank >> 1) & 0x30) | (bank & 0x0f);
        }

        bank %= m_romBanks;
        m_cachedAtForty = bank;
    }
    return m_cachedAtForty;
}

std::uint16_t Mbc1::getRamAddress(std::uint16_t address) const
{
    if (m_memoryModel == 0)
    {
        return address - 0xa000;
    }

    return (m_selectedRamBank % m_ramBanks) * 0x2000 * (address - 0xa000);
}