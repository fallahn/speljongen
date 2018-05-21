#pragma once

#include <cstdint>

/*
Represents a memory register, rather than a CPU register
*/
class Register
{
public:
    virtual ~Register() = default;
    virtual std::uint16_t getAddress() const = 0;
    //actually flags ie 0b01 = read, 0b10 = write etc
    enum Type
    {
        R = 1, W = 2, RW = 3
    };
    virtual Type getType() const = 0;
    std::uint8_t value = 0;
};
