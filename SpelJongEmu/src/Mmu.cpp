#include "Mmu.hpp"
#include "BootRom.hpp"
#include "MemoryRegisters.hpp"

#include <cstring>
#include <iostream>

namespace
{
    const std::vector<std::uint8_t> logo =
    {
        0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
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
bool Mmu::accepts(std::uint16_t address) const
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

    //temp
    if (address == MemoryRegisters::SC && value == 0x81)
    {
        std::cout << getByte(MemoryRegisters::SB);
    }
}

std::uint8_t Mmu::getByte(std::uint16_t address) const
{
    if (m_addressSpaces[address]) return m_addressSpaces[address]->getByte(address);
    return getStorageValue(address);
}

void Mmu::addAddressSpace(AddressSpace& space)
{
    for (auto i = 0u; i < m_addressSpaces.size(); ++i)
    {
        if (space.accepts(i))
        {
            m_addressSpaces[i] = &space;
        }
    }
}

void Mmu::initBios()
{
    std::uint16_t address = 0;
    for (auto b : BootRom::CLASSIC)
    {
        setByte(address++, b);
    }

    address = 0x104;
    for (auto b : logo)
    {
        setByte(address++, b);
    }
}