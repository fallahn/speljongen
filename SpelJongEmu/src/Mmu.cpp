#include "Mmu.hpp"
#include "BootRom.hpp"

#include <cstring>

namespace
{
    const std::vector<std::uint8_t> logo =
    {
        0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
    };
}

Mmu::Mmu()
    :m_storage(0xffff)
{
    //TODO check if GBC
    //logo code at 0x104 so we see something when no game inserted - this is normally
    //part of the ROM data.
    std::memcpy(m_storage.data(), BootRom::CLASSIC.data(), BootRom::CLASSIC.size());
    std::memcpy(m_storage.data() + 0x104, logo.data(), logo.size());
}

//public
bool Mmu::accepts(std::uint16_t address) const
{
    return true;
}

void Mmu::setByte(std::uint16_t address, std::uint8_t value)
{   
    //I don't like having to do this but some spaces such
    //as the GPU have special accessors
    for (auto& space : m_spaces)
    {
        if (space->accepts(address))
        {
            space->setByte(address, value);
            return;
        }
    }
    
    m_storage[address] = value;
}

std::uint8_t Mmu::getByte(std::uint16_t address) const
{
    for (auto& space : m_spaces)
    {
        if (space->accepts(address))
        {
            return space->getByte(address);
        }
    }

    return m_storage[address];
}