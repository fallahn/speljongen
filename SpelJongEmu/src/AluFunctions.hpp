#pragma once

#include <cstdint>

class Flags;
namespace AluFunctions
{
    static inline std::uint8_t inc(Flags&, std::uint8_t);
    static inline std::uint16_t inc16(Flags&, std::uint16_t);
    static inline std::uint8_t dec(Flags&, std::uint8_t);
    static inline std::uint16_t dec16(Flags&, std::uint16_t);
    static inline std::uint16_t add(Flags&, std::uint16_t, std::uint16_t);
    static inline std::uint16_t add(Flags&, std::uint16_t, std::int8_t);
    static inline std::uint16_t add_sp(Flags&, std::uint16_t, std::int8_t);
    static inline std::uint8_t daa(Flags&, std::uint8_t);
    static inline std::uint8_t cpl(Flags&, std::uint8_t);
    static inline std::uint8_t scf(Flags&, std::uint8_t);
    static inline std::uint8_t ccf(Flags&, std::uint8_t);
    static inline std::uint8_t add(Flags&, std::uint8_t, std::uint8_t);
    static inline std::uint8_t adc(Flags&, std::uint8_t, std::uint8_t);
    static inline std::uint8_t sub(Flags&, std::uint8_t, std::uint8_t);
    static inline std::uint8_t sbc(Flags&, std::uint8_t, std::uint8_t);
    static inline std::uint8_t AND(Flags&, std::uint8_t, std::uint8_t);
    static inline std::uint8_t OR(Flags&, std::uint8_t, std::uint8_t);
    static inline std::uint8_t XOR(Flags&, std::uint8_t, std::uint8_t);
    static inline std::uint8_t cp(Flags&, std::uint8_t, std::uint8_t);
    static inline std::uint8_t rlc(Flags&, std::uint8_t);
    static inline std::uint8_t rrc(Flags&, std::uint8_t);
    static inline std::uint8_t rl(Flags&, std::uint8_t);
    static inline std::uint8_t rr(Flags&, std::uint8_t);
    static inline std::uint8_t sla(Flags&, std::uint8_t);
    static inline std::uint8_t sra(Flags&, std::uint8_t);
    static inline std::uint8_t swap(Flags&, std::uint8_t);
    static inline std::uint8_t srl(Flags&, std::uint8_t);
    static inline std::uint8_t bit(Flags&, std::uint8_t, std::uint8_t);
    static inline std::uint8_t res(std::uint8_t, std::uint8_t);
    static inline std::uint8_t set(std::uint8_t, std::uint8_t);
};
