#pragma once

#include <array>
#include <cstdint>

namespace BitUtil
{
    static inline std::uint8_t getMSB(std::uint16_t word)
    {
        return word >> 8;
    }

    static inline std::uint8_t getLSB(std::uint16_t word)
    {
        return word & 0xff;
    }

    static inline std::uint16_t toWord(std::uint8_t msb, std::uint8_t lsb)
    {
        return (msb << 8) | lsb;
    }

    static inline std::uint16_t toWord(std::array<std::uint8_t, 2u> bytes)
    {
        return toWord(bytes[1], bytes[0]);
    }

    static inline bool getBit(std::uint8_t byte, std::size_t position)
    {
        return (byte & (1 << position)) != 0;
    }

    static inline std::uint8_t clearBit(std::uint8_t byte, std::size_t position)
    {
        return byte & ~(1 << position);
    }

    static inline std::uint8_t setBit(std::uint8_t byte, std::size_t position)
    {
        return byte | (1 << position);
    }

    static inline std::int8_t toSigned(std::uint8_t byte)
    {
        if ((byte & (1 << 7)) == 0)
        {
            return byte;
        }

        return byte - 0x100;
    }
}