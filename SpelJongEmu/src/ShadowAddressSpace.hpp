#pragma once

//implements RAM shadowing.
//writing to this space copies to the RAM it shadows - not the other way.
//by default this copies the data by (0xe000 - 0xc000), the difference in
//addresses on a gameboy.

#include "AddressSpace.hpp"

class ShadowAddressSpace final : public AddressSpace
{
public:
    ShadowAddressSpace(std::vector<std::uint8_t>& storage, std::uint16_t echoStart, std::uint16_t targetStart, std::uint16_t length)
        : AddressSpace(storage), m_echoStart(echoStart), m_targetStart(targetStart), m_echoEnd(m_echoStart + length)
    {
        assert(m_echoStart < m_echoEnd);
    }

    std::string getLabel() const override { return "Shadow Space"; }

    bool accepts(std::uint16_t address) const override
    {
        return address >= m_echoStart && address < m_echoEnd;
    }

    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        assert(accepts(address));
        setStorageValue(mapAddress(address), value);
        setStorageValue(address, value);
    }

    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        //return getStorage()[mapAddress(address)];
        return getStorageValue(address);
    }

private:
    std::uint16_t m_echoStart;
    std::uint16_t m_targetStart;
    std::uint16_t m_echoEnd;

    //maps the echo location to the target
    std::uint16_t mapAddress(std::uint16_t address) const
    {
        address -= m_echoStart;
        address += m_targetStart;
        return address;
    }
};
