#include "Mmu.hpp"
#include "MemoryRegisters.hpp"

#include <cstring>
#include <iostream>
#include <random>
#include <ctime>
#include <iomanip>
#include <string>

namespace
{
    std::mt19937 randEngine(static_cast<unsigned long>(std::time(nullptr)));

    const std::vector<std::uint8_t> fakeBios = 
    {
        0x21, //ld hl d16
        0x04,
        0x01,
        0x46, //ld b, (hl)
        0x3E, //ld a, d8
        0xce, //first byte of logo
        0x90, //SUB b
        0xca, //jp z a16
        0x0d,
        0x00, //jumps to 0x000d if SUB was 0
        0xc3, //jp a16
        0x00, 0x00, //return to beginning
        0x3e, 0x01, //reset register A
        0x06, 0x00, //reset register B
        0x21, 0x50, 0xff, //load hl with 'ok' address
        0x77, //write '1' (from A) to 'ok' address
        0xc3, 0x00, 0x01 //jp 0x0100
    };
}

Mmu::Mmu(std::vector<std::uint8_t>& storage)
    : AddressSpace(storage),
    m_addressSpaces(0x10000)
{
    //TODO check if GBC
    for (auto& s : m_addressSpaces) s = nullptr;
}

//public
bool Mmu::accepts(std::uint16_t) const
{
    return true;
}

void Mmu::setByte(std::uint16_t address, std::uint8_t value)
{          
    if (m_addressSpaces[address])
    {
        m_addressSpaces[address]->setByte(address, value);
    }
    else
    {
        setStorageValue(address, value);
    }

    //temp - used with test roms to print output to console
    /*if (address == MemoryRegisters::SC && value == 0x81)
    {
        std::cout << getByte(MemoryRegisters::SB);
    }*/
}

std::uint8_t Mmu::getByte(std::uint16_t address) const
{    
    if (m_addressSpaces[address]) return m_addressSpaces[address]->getByte(address);
    return getStorageValue(address);
}

void Mmu::reset()
{
    std::uniform_int_distribution<int> randDist(0, 255);
    for (auto i = 0u; i < m_addressSpaces.size(); ++i)
    {
        m_addressSpaces[i] = nullptr;
        //setStorageValue(static_cast<std::uint16_t>(i), static_cast<std::uint8_t>(randDist(randEngine)));
        setStorageValue(i, 0);
    }
}

void Mmu::addAddressSpace(AddressSpace& space)
{
    for (auto i = 0u; i < m_addressSpaces.size(); ++i)
    {
        if (space.accepts(i) && m_addressSpaces[i] != &space)
        {
            if (m_addressSpaces[i] != nullptr)
            {
                std::cout << "Warning, overwrote previously mapped address space " << m_addressSpaces[i]->getLabel() << " at "
                    << std::hex << std::setfill('0') << std::setw(4) << i << " with " << space.getLabel() << "\n";
            }

            m_addressSpaces[i] = &space;
        }
    }
}

void Mmu::initBios()
{
    std::uint16_t address = 0;

    //fake bios which checks if cart loaded
    //and halts if not (see anon namespace)
    for (auto b : fakeBios)
    {
        setByte(address++, b);
    }
}

void Mmu::removeCartridge()
{
    for (auto i = 0; i < 0x8000; ++i)
    {
        m_addressSpaces[i] = nullptr;
    }
}

void Mmu::insertCartridge(AddressSpace& cartridge)
{
    addAddressSpace(cartridge);
}