#pragma once

#include "AddressSpace.hpp"

class ColourRam final : public AddressSpace
{
public:
    explicit ColourRam(std::vector<std::uint8_t>& storage)
        : AddressSpace(storage) {}

    std::string getLabel() const override { return "Colour RAM"; }

    bool accepts(std::uint16_t address) const override
    {
        return address == 0xff70 || (address >= 0xd000 && address < 0xe000);
    }

    void setByte(std::uint16_t address, std::uint8_t value)
    {
        assert(accepts(address));
        setStorageValue(address, value);
    }

    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        return getStorageValue(address);
    }

private:

};