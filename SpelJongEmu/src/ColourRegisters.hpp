#pragma once

#include "AddressSpace.hpp"

class ColourRegisters final : public AddressSpace
{
public:
    explicit ColourRegisters(std::vector<std::uint8_t>& storage)
        : AddressSpace(storage)
    {
        storage[0xff6c] = 0xfe;
        storage[0xff74] = 0xff;
        storage[0xff75] = 0x8f;
    }

    void reset()
    {
        setByte(0xff6c, 0);
        setByte(0xff74, 0xff);
        setByte(0xff75, 0);
    }

    std::string getLabel() const override { return "Colour Registers"; }

    bool accepts(std::uint16_t address) const override
    {
        return address == 0xff6c || (address >= 0xff72 && address < 0xff78);
    }

    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        assert(accepts(address));
        switch (address)
        {
        case 0xff6c:
            setStorageValue(address, 0xfe | (value & 0x01));
            break;
        case 0xff75:
            setStorageValue(address, 0x8f | (value & 0x70));
            break;
        default:
            setStorageValue(address, value);
            break;
        }
    }

    std::uint8_t getByte(std::uint16_t address) const
    {
        assert(accepts(address));
        return getStorageValue(address);
    }
};