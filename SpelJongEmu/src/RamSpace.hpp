#pragma once
/*
Differs from RAM class by echoing the bytes written to
the shadow space address
*/

#include "AddressSpace.hpp"

class RamSpace final : public AddressSpace
{
public:
    RamSpace(std::vector<std::uint8_t>& storage, std::uint16_t start, std::uint16_t size, std::uint16_t echoOffset)
        : AddressSpace(storage),
        m_startAddress(start), m_endAddress(start + size), m_echoOffset(echoOffset)
    {
        assert(m_startAddress < m_endAddress);
    }

    std::string getLabel() const override { return "Shadowed RAM"; }

    bool accepts(std::uint16_t address) const override
    {
        return (address >= m_startAddress && address < m_endAddress);
    }

    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        assert(accepts(address));
        setStorageValue(address, value);
        setStorageValue(address + m_echoOffset, value);
    }

    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        return getStorageValue(address);
    }

private:
    std::uint16_t m_startAddress;
    std::uint16_t m_endAddress;
    std::uint16_t m_echoOffset;
};