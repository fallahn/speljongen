#pragma once

#include "AddressSpace.hpp"

class InterruptManager;
class SpeedMode;
class Timer final : public AddressSpace
{
public:
    Timer(std::vector<std::uint8_t>&, InterruptManager&, const SpeedMode&);

    void tick();
    bool accepts(std::uint16_t) const override;
    void setByte(std::uint16_t, std::uint8_t) override;
    std::uint8_t getByte(std::uint16_t) const override;

private:
    InterruptManager & m_interruptManager;
    const SpeedMode& m_speedMode;

    std::uint16_t m_div;
    std::uint16_t m_tac;
    std::uint16_t m_tma;
    std::uint16_t m_tima;

    bool m_prevBit;
    bool m_overflow;

    std::int32_t m_ticksSinceOverflow;

    void incTma();
    void updateDiv(std::uint16_t);
};