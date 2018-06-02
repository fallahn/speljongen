#pragma once

//creates an address space used as RAM.
//requires storage to be set by MMU

//TODO when writing to RAM 0c000 - 0dfff the data
//should be shadowed by + 0x2000

#include "AddressSpace.hpp"

#include <cassert>
#include <iostream>

class Ram final : public AddressSpace
{
public:
    Ram(std::vector<std::uint8_t>& storage, std::uint16_t start, std::uint16_t length, bool sharedStorage = true)
        : AddressSpace  (storage),
        m_start         (start),
        m_end           (start + length),
        m_useInternal   (false)
    {
        assert(m_end > m_start); //values have wrpapped around by becoming negative or something :S
        if (!sharedStorage)
        {
            //set storage to local
            m_ownStorage.resize(m_end); //it's a bit of a waste, but meh
            m_useInternal = true;
        }
    }

    Type type() const override { return AddressSpace::Type::Ram; }

    bool accepts(std::uint16_t address) const override
    {
        return (address >= m_start && address < m_end);
    }

    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        assert(accepts(address));
        if (m_useInternal)
        {
            m_ownStorage[address] = value;
            return;
        }
        getStorage()[address] = value;
    }

    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        if (m_useInternal)
        {
            return m_ownStorage[address];
        }
        return getStorage()[address];
    }

private:
    std::uint16_t m_start = 0;
    std::uint16_t m_end = 0;
    std::vector<std::uint8_t> m_ownStorage;
    bool m_useInternal;
    //bool wasWritten = false;
};