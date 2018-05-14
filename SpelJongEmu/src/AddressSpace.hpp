#pragma once

#include <cstdint>

class AddressSpace
{
public:
    virtual ~AddressSpace() = default;

    virtual bool accepts(std::uint16_t address) const = 0;

    virtual void setByte(std::uint16_t address, std::uint8_t value) = 0;

    virtual std::uint8_t getByte(std::uint16_t address) const = 0;
};