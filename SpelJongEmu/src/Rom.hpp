#pragma once

#include "AddressSpace.hpp"

class Rom final : public AddressSpace
{
public:
    explicit Rom(std::vector<std::uint8_t>& storage, const std::vector<char>& buf)
        : AddressSpace(storage)
    {
        for (auto i = 0u; i < buf.size() && i < 0x8000; ++i)
        {
            storage[i] = buf[i];
        }
    }

    bool accepts(std::uint16_t address) const override
    {
        return (address < 0x8000) ||
            (address >= 0xa000 && address < 0xc000);
    }

    void setByte(std::uint16_t, std::uint8_t) override
    {
        //assert(false);
    }

    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        return getStorageValue(address);
    }
};
