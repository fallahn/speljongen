#pragma once

#include "Register.hpp"

#include <vector>

class GpuRegister final : public Register
{
public:
    GpuRegister(std::uint16_t address = 0, Register::Type type = RW)
        : m_address(address), m_type(type) {}

    std::uint16_t getAddress() const override { return m_address; }
    Register::Type getType() const override { return m_type; }

    enum ID
    {
        STAT, SCY, SCX, LY, LYC, BGP,
        OBP0, OBP1, WY, WX, VBK
    };

    static const std::vector<GpuRegister> registers;

private:
    std::uint16_t m_address;
    Register::Type m_type;
};