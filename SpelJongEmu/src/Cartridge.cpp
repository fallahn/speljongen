#include "Cartridge.hpp"
#include "Mbc1.hpp"
#include "Mbc3.hpp"
#include "Mbc5.hpp"
#include "Rom.hpp"
#include "BootRom.hpp"
#include "zip_file.hpp"

#include <iostream>
#include <fstream>

Cartridge::Cartridge(std::vector<std::uint8_t>& storage)
    : AddressSpace      (storage),
    m_info              (ROM),
    m_colour            (false),
    m_title             ("NO TITLE"),
    m_infoStr           ("\nNo cartridge loaded"),
    m_romBanks          (0),
    m_ramBanks          (0),
    m_selectedRomBank   (1),
    m_type              (Classic)
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

bool Cartridge::load(const std::string& path)
{
    std::cout << "loading: " << path << "\n";

    std::vector<char> buf;

    if (path.find(".zip") != std::string::npos)
    {
        //we have a zip file
        std::cout << "opening zip file\n";

        miniz_cpp::zip_file file(path);
        std::string romName;
        
        auto filelist = file.namelist();
        for (const auto& name : filelist)
        {
            if (name.find(".GB") != std::string::npos
                || name.find(".gb") != std::string::npos
                || name.find(".GBC") != std::string::npos
                || name.find(".gbc") != std::string::npos)
            {
                romName = name;
                break;
            }
        }

        if (romName.empty())
        {
            std::cout << "No ROM file was found in archive\n";
            return false;
        }

        auto fileString = file.read(romName);
        for (auto c : fileString)
        {
            buf.push_back(c);
        }
    }
    else
    {
        //TODO some file size checking
        std::ifstream file(path, std::ios::binary);

        file.seekg(0, file.end);
        size_t length = file.tellg();
        file.seekg(0, file.beg);
        buf.resize(length);
        file.read(buf.data(), length);
        file.close();
    }

    m_selectedRomBank = 1;
    m_infoStr = "\nCartridge Info:\nTitle: ";

    //grab title
    m_title.clear();
    for (auto i = 0x0134; i < 0x0143; ++i)
    {
        if (buf[i] == 0)
        {
            break;
        }
        m_title.push_back(buf[i]);
    }
    if (m_title.empty())
    {
        m_title = "NO TITLE";
    }

    m_infoStr += m_title + "\n";

    auto b = static_cast<std::uint8_t>(buf[0x0143]);
    switch (b) //colour or universal?
    {
    default:
        m_type = Type::Classic;
        m_infoStr += "Compat: Classic\n";
        m_colour = false;
        break;
    case 0x80:
        m_type = Type::Universal;
        m_infoStr += "Compat: Universal\n";
        m_colour = true;
        break;
    case 0xc0:
        m_type = Type::Colour;
        m_infoStr += "Compat: Colour Gameboy\n";
        m_colour = true;
        break;
    }

    m_romBanks = static_cast<std::uint8_t>(buf[0x0148]);
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

    m_infoStr += "ROM Banks: " + std::to_string(m_romBanks) + "\n";

    m_ramBanks = static_cast<std::uint8_t>(buf[0x0149]);
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

    m_infoStr += "RAM Banks: " + std::to_string(m_ramBanks) + "\n";

    m_info = static_cast<Info>(static_cast<std::uint8_t>(buf[0x0147]));
    if (m_info >= ROM_MBC1 && m_info <= ROM_MBC1_RAM_BATTERY)
    {
        m_mbc = std::make_unique<Mbc1>(getStorage(), buf, m_romBanks, m_ramBanks, m_selectedRomBank);
        switch (m_info)
        {
        default:
            m_infoStr += "Type: MBC1\n";
            break;
        case ROM_MBC1_RAM:
            m_infoStr += "Type: MBC1 with RAM\n";
            break;
        case ROM_MBC1_RAM_BATTERY:
            m_infoStr += "Type: MBC1 with RAM\nand battery\n";
            break;
        }
    }
    else if(m_info >= ROM_MBC5 && m_info <= ROM_MBC5_RUMBLE_SRAM_BATTERY)
    {
        m_mbc = std::make_unique<Mbc5>(getStorage(), buf, m_romBanks, m_ramBanks, m_selectedRomBank);
        switch (m_info)
        {
        default:
            m_infoStr += "Type: MBC5\n";
            break;
        case ROM_MBC5_RAM:
            m_infoStr += "Type: MBC5 with RAM\n";
            break;
        case ROM_MBC5_RAM_BATTERY:
            m_infoStr += "Type: MBC5 with RAM\nand battery\n";
            break;
        case ROM_MBC5_RUMBLE:
            m_infoStr += "Type: MBC5 with rumble\n";
            break;
        case ROM_MBC5_RUMBLE_SRAM:
            m_infoStr += "Type: MBC5 with rumble\nand SRAM\n";
            break;
        case ROM_MBC5_RUMBLE_SRAM_BATTERY:
            m_infoStr += "Type: MBC5 with rumble,\nSRAM and battery\n";
            break;
        }
    }
    else if (m_info >= ROM_MBC3 && m_info <= ROM_MBC3_RAM_BATTERY)
    {
        m_mbc = std::make_unique<Mbc3>(getStorage(), buf, m_ramBanks, m_selectedRomBank);
        switch (m_info)
        {
        default:
            m_infoStr += "Type: MBC3\n";
            break;
        case ROM_MBC3_RAM:
            m_infoStr += "Type: MBC3 with RAM\n";
            break;
        case ROM_MBC3_RAM_BATTERY:
            m_infoStr += "Type: MBC3 with RAM\nand battery\n";
            break;
        case ROM_MBC3_TIMER_BATTERY:
            m_infoStr += "Type: MBC3 with RTC\nand battery\n";
            break;
        case ROM_MBC3_TIMER_RAM_BATTERY:
            m_infoStr += "Type: MBC3 with RTC,\nRAM and battery\n";
            break;
        }
    }
    else
    {
        m_mbc = std::make_unique<Rom>(getStorage(), buf);
        if (m_info > 0)
        {
            std::cout << "WARNING incorrect MBC type loaded for cartridge\n";
            m_infoStr += "MBC type unknown!\n";
        }
        else
        {
            m_infoStr += "MBC Type: ROM\n";
        }
    }

    return true;
}