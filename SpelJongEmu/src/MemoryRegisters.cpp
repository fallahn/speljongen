#include "MemoryRegisters.hpp"

namespace
{
    //const std::uint16_t RegistersStart = 0xff00;
    //const std::uint16_t RegistersEnd = 0xff50;
}

MemoryRegisters::MemoryRegisters(std::vector<std::uint8_t>& storage, std::uint16_t start, std::uint16_t end)
    : AddressSpace  (storage),
    m_start         (start),
    m_end           (end)
{
    for (auto i = 0; i < 0x4c; ++i)
    {
        m_readFlags.push_back(ReadWrite);
    }

    //TODO timer flags shoulld probably be read-only from here
    //as they are set by the timer class

    m_readFlags[0x18] = Write; //NR23
    m_readFlags[0x1D] = Write; //NR33
    //m_readFlags[0x44] = Read; //LY
    m_readFlags[0x46] = Write; //DMA
    m_readFlags[0x4f] = Write; //VBK
}

//public
bool MemoryRegisters::accepts(std::uint16_t address) const
{
    return address >= m_start && address <= m_end;
}

void MemoryRegisters::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address)); //&& (m_readFlags[address - RegistersStart] & Write));

    setStorageValue(address, value);
}

std::uint8_t MemoryRegisters::getByte(std::uint16_t address) const
{
    assert(accepts(address)); //&& (m_readFlags[address - RegistersStart] & Read));

    return getStorageValue(address);
}

std::uint8_t MemoryRegisters::preIncrement(std::uint16_t address)
{
    assert(accepts(address)); //&& (m_readFlags[address - RegistersStart] & Write));

    std::uint8_t value = getStorageValue(address);
    value++;
    setStorageValue(address, value);
    return value;
}

void MemoryRegisters::reset()
{
    for (auto i = m_start; i < m_end; ++i)
    {
        setStorageValue(i, 0);
    }
}