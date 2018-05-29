#include "Cartridge.hpp"
#include "Mbc1.hpp"

#include <iostream>
#include <fstream>

Cartridge::Cartridge(std::vector<std::uint8_t>& storage)
    : AddressSpace  (storage),
    m_info          (ROM),
    m_colour        (false),
    m_romBanks      (0),
    m_ramBanks      (0),
    m_type          (Classic)
{

}

//public
bool Cartridge::accepts(std::uint16_t address) const
{
    return m_mbc->accepts(address) || address == 0xff50; //BIOS writes this when cartidge check passes
}

void Cartridge::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    if (address == 0xff50)
    {
        setStorageValue(address, value);
    }
    else
    {
        m_mbc->setByte(address, value);
    }
}

std::uint8_t Cartridge::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    if (address == 0xff50) return getStorageValue(address);
    else return m_mbc->getByte(address);

    //TODO could check not booted yet and return BIOS instead
}

void Cartridge::load(const std::string& path)
{
    std::cout << "loading: " << path << "\n";
    //TODO some file size checking
    std::ifstream file(path, std::ios::binary);

    file.seekg(0, file.end);
    size_t length = file.tellg();
    file.seekg(0, file.beg);
    std::vector<char> buf(length);
    file.read(buf.data(), length);
    file.close();

    //grab title
    m_title.clear();
    for (auto i = 0x0134; i < 0x0143; ++i)
    {
        m_title.push_back(buf[i]);
        if (buf[i] == 0)
        {
            break;
        }
    }

    //m_type = buf[0x0143]; //colour or universal?

    m_romBanks = buf[0x0148];
    switch (m_romBanks)
    {
    default:
        std::cout << "Invalid ROM bank size!\n";
        break;
    case 0: m_romBanks = 2; break;
    case 1: m_romBanks = 4; break;
    case 2: m_romBanks = 8; break;
    case 3: m_romBanks = 16; break;
    case 4: m_romBanks = 32; break;
    case 5: m_romBanks = 64; break;
    case 6: m_romBanks = 128; break;
    case 7: m_romBanks = 256; break;
    case 0x52: m_romBanks = 72; break;
    case 0x53: m_romBanks = 80; break;
    case 0x54: m_romBanks = 96; break;
    }

    m_ramBanks = buf[0x0149];
    switch (m_ramBanks)
    {
    default:
        std::cout << "Invalid ram bank size\n";
        break;
    case 0:
    case 1:
        break;
    case 2: m_ramBanks = 1;
        break;
    case 3:
        m_ramBanks = 4;
        break;
    case 4:
        m_ramBanks = 16;
        break;
    }

    m_info = static_cast<Info>(buf[0x0147]);
    if (m_info >= ROM_MBC1 && m_info <= ROM_MBC1_RAM_BATTERY)
    {
        m_mbc = std::make_unique<Mbc1>(getStorage(), buf, m_romBanks, m_ramBanks);
    }
    else
    {
        std::cout << "MBC type not yet implemented!\n";
    }
}