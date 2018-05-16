#include "AluFunctions.hpp"
#include "Registers.hpp"

#include <cassert>


std::uint8_t AluFunction::inc(Flags& flags, std::uint8_t arg)
{
    std::uint8_t result = arg + 1;
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, ((arg & 0x0f) == 0x0f));
    return result;
}

std::uint16_t AluFunction::inc16(Flags& flags, std::uint16_t arg)
{
    return arg + 1;
}

std::uint8_t AluFunction::dec(Flags& flags, std::uint8_t arg)
{
    std::uint8_t result = arg - 1;
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, true);
    flags.set(Flags::H, ((arg & 0x0f) == 0));
    return result;
}

std::uint16_t AluFunction::dec16(Flags& flags, std::uint16_t arg)
{
    return arg - 1;
}

std::uint16_t AluFunction::add(Flags& flags, std::uint16_t arg1, std::uint16_t arg2)
{
    flags.set(Flags::N, false);
    flags.set(Flags::H, ((arg1 & 0x0fff) + (arg2 & 0x0fff) > 0x0fff));
    flags.set(Flags::C, arg1 + arg2 > 0xffff);
    return arg1 + arg2;
}

std::uint16_t AluFunction::add(Flags& flags, std::uint16_t arg1, std::int8_t arg2)
{
    return arg1 + arg2;
}

std::uint16_t AluFunction::add_sp(Flags& flags, std::uint16_t arg1, std::int8_t arg2)
{
    flags.set(Flags::Z, false);
    flags.set(Flags::N, false);

    uint16_t result = arg1 + arg2;
    flags.set(Flags::C, (((arg1 & 0xff) + (arg2 & 0xff)) & 0x100) != 0);
    flags.set(Flags::H, (((arg1 & 0x0f) + (arg2 & 0x0f)) & 0x10) != 0);
    return result;
}

std::uint8_t AluFunction::daa(Flags& flags, std::uint8_t arg)
{
    std::int32_t result = arg;
    if (flags.isSet(Flags::N))
    {
        if (flags.isSet(Flags::H))
        {
            result = (result - 6) & 0xff;
        }
        if (flags.isSet(Flags::C))
        {
            result = (result - 0x60) & 0xff;
        }
    }
    else
    {
        if (flags.isSet(Flags::H) || (result & 0xf) > 9)
        {
            result += 0x06;
        }
        if (flags.isSet(Flags::C) || result > 0x9f)
        {
            result += 0x60;
        }
    }
    flags.set(Flags::H, false);

    if (result > 0xff)
    {
        flags.set(Flags::C, true);
    }

    result &= 0xff;
    flags.set(Flags::Z, result == 0);
    return static_cast<std::uint8_t>(result);
}

std::uint8_t AluFunction::cpl(Flags& flags, std::uint8_t arg)
{
    flags.set(Flags::N, true);
    flags.set(Flags::H, true);
    return ~arg;
}

std::uint8_t AluFunction::scf(Flags& flags, std::uint8_t arg)
{
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    flags.set(Flags::C, true);
    return arg;
}

std::uint8_t AluFunction::ccf(Flags& flags, std::uint8_t arg)
{
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    flags.set(Flags::C, !flags.isSet(Flags::C));
    return arg;
}

std::uint8_t AluFunction::add(Flags& flags, std::uint8_t arg1, std::uint8_t arg2)
{
    std::int32_t sum = arg1 + arg2;
    flags.set(Flags::Z, (sum & 0xff) == 0);
    flags.set(Flags::N, false);
flags.set(Flags::H, (arg1 & 0x0f) + (arg2 & 0x0f) > 0x0f);
flags.set(Flags::C, sum > 0xff);
return static_cast<std::uint8_t>(sum & 0xff);
}

std::uint8_t AluFunction::adc(Flags& flags, std::uint8_t arg1, std::uint8_t arg2)
{
    std::uint8_t carry = flags.isSet(Flags::C) ? 1 : 0;
    std::int32_t sum = arg1 + arg2 + carry;
    flags.set(Flags::Z, (sum & 0xff) == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, (arg1 & 0x0f) + (arg2 & 0x0f) + carry > 0x0f);
    flags.set(Flags::C, sum > 0xff);
    return static_cast<std::uint8_t>(sum & 0xff);
}

std::uint8_t AluFunction::sub(Flags& flags, std::uint8_t arg1, std::uint8_t arg2)
{
    std::int32_t sum = arg1 - arg2;
    flags.set(Flags::Z, (sum & 0xff) == 0);
    flags.set(Flags::N, true);
    flags.set(Flags::H, (arg2 & 0x0f) > (arg1 & 0x0f));
    flags.set(Flags::C, (arg2 > arg1));
    return static_cast<std::uint8_t>(sum & 0xff);
}

std::uint8_t AluFunction::sbc(Flags& flags, std::uint8_t arg1, std::uint8_t arg2)
{
    std::uint8_t carry = flags.isSet(Flags::C) ? 1 : 0;
    std::int32_t sum = arg1 - arg2 - carry;

    flags.set(Flags::Z, (sum & 0xff) == 0);
    flags.set(Flags::N, true);
    flags.set(Flags::H, ((arg1 ^ arg2 ^ (sum & 0xff)) & (1 << 4)) != 0);
    flags.set(Flags::C, sum < 0);
    return static_cast<std::uint8_t>(sum & 0xff);
}

std::uint8_t AluFunction::AND(Flags& flags, std::uint8_t arg1, std::uint8_t arg2)
{
    std::uint8_t result = arg1 & arg2;
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, true);
    flags.set(Flags::C, false);
    return result;
}

std::uint8_t AluFunction::OR(Flags& flags, std::uint8_t arg1, std::uint8_t arg2)
{
    std::uint8_t result = arg1 | arg2;
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    flags.set(Flags::C, false);
    return result;
}

std::uint8_t AluFunction::XOR(Flags& flags, std::uint8_t arg1, std::uint8_t arg2)
{
    std::uint8_t result = arg1 ^ arg2;
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    flags.set(Flags::C, false);
    return result;
}

std::uint8_t AluFunction::cp(Flags& flags, std::uint8_t arg1, std::uint8_t arg2)
{
    std::int32_t cp = (arg1 - arg2) & 0xff;
    flags.set(Flags::Z, cp == 0);
    flags.set(Flags::N, true);
    flags.set(Flags::H, (arg2 & 0x0f) > (arg1 & 0x0f));
    flags.set(Flags::C, arg2 > arg1);
    return arg1;
}

std::uint8_t AluFunction::rlc(Flags& flags, std::uint8_t arg)
{
    std::int32_t result = (arg << 1) & 0xff;
    if ((arg & (1 << 7)) != 0)
    {
        result |= 1;
        flags.set(Flags::C, true);
    }
    else
    {
        flags.set(Flags::C, false);
    }
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    return static_cast<std::uint8_t>(result);
}

std::uint8_t AluFunction::rrc(Flags& flags, std::uint8_t arg)
{
    std::int32_t result = arg >> 1;
    if ((arg & 1) == 1)
    {
        result |= (1 << 7);
        flags.set(Flags::C, true);
    }
    else
    {
        flags.set(Flags::C, false);
    }
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    return static_cast<std::uint8_t>(result);
}

std::uint8_t AluFunction::rl(Flags& flags, std::uint8_t arg)
{
    std::int32_t result = (arg << 1) & 0xff;
    result |= flags.isSet(Flags::C) ? 1 : 0;
    flags.set(Flags::C, (arg & (1 << 7)) != 0);
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    return static_cast<std::uint8_t>(result);
}

std::uint8_t AluFunction::rr(Flags& flags, std::uint8_t arg)
{
    std::int32_t result = arg >> 1;
    result |= flags.isSet(Flags::C) ? (1 << 7) : 0;
    flags.set(Flags::C, (arg & 1) != 0);
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    return static_cast<std::uint8_t>(result);
}

std::uint8_t AluFunction::sla(Flags& flags, std::uint8_t arg)
{
    std::uint8_t result = arg << 1;
    flags.set(Flags::C, (arg & (1 << 7)) != 0);
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    return result;
}

std::uint8_t AluFunction::sra(Flags& flags, std::uint8_t arg)
{
    std::uint8_t result = (arg >> 1) | (arg & (1 << 7));
    flags.set(Flags::C, (arg & 1) != 0);
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    return result;
}

std::uint8_t AluFunction::swap(Flags& flags, std::uint8_t arg)
{
    std::uint8_t upper = arg & 0xf0;
    std::uint8_t lower = arg & 0x0f;
    std::uint8_t result = (upper >> 4) | (lower << 4);
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    flags.set(Flags::C, false);
    return result;
}

std::uint8_t AluFunction::srl(Flags& flags, std::uint8_t arg)
{
    std::uint8_t result = arg >> 1;
    flags.set(Flags::C, (arg & 1) != 0);
    flags.set(Flags::Z, result == 0);
    flags.set(Flags::N, false);
    flags.set(Flags::H, false);
    return result;
}

std::uint8_t AluFunction::bit(Flags& flags, std::uint8_t arg1, std::uint8_t arg2)
{
    flags.set(Flags::N, false);
    flags.set(Flags::H, true);
    if (arg2 < 8)
    {
        flags.set(Flags::Z, !BitUtil::getBit(arg1, arg2));
    }
    return arg1;
}

std::uint8_t AluFunction::res(std::uint8_t arg1, std::uint8_t arg2)
{
    return arg1 & ~(1 << arg2);
}

std::uint8_t AluFunction::set(std::uint8_t arg1, std::uint8_t arg2)
{
    return arg1 |= (1 << arg2);
}