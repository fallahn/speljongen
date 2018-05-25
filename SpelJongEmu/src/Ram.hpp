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
    Ram(std::uint16_t start, std::uint16_t length, bool sharedStorage = true)
        : m_start   (start),
        m_end       (start + length)
    {
        assert(m_end > m_start); //values have wrpapped around by becoming negative or something :S
        if (!sharedStorage)
        {
            //set storage to local
            m_ownStorage.resize(m_end); //it's a bit of a waste, but meh
            setStorage(m_ownStorage);
            ownStorage = true;
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
        getStorage()[address] = value;
        //if (!ownStorage && value > 0) std::cout << "Wrote to: " << address << ": " << (int)value << "\n";
        //if (ownStorage && value > 0) wasWritten = true;
    }

    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        //if (!ownStorage /*&& wasWritten*/&& getStorage()[address] > 0) std::cout << "Read from: " << address << ": " << (int)getStorage()[address] << "\n";
        return getStorage()[address];
    }

private:
    std::uint16_t m_start = 0;
    std::uint16_t m_end = 0;
    std::vector<std::uint8_t> m_ownStorage;
    bool ownStorage = false;
    bool wasWritten = false;
};