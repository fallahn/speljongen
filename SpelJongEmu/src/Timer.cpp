#include "Timer.hpp"
#include "InterruptManager.hpp"
#include "SpeedMode.hpp"

#include <array>
#include <cassert>

namespace
{
    constexpr std::array<std::int32_t, 4u> FreqToBit = { 9, 3, 5, 7 };
}

Timer::Timer(std::vector<std::uint8_t>& storage, InterruptManager& im, const SpeedMode& sm)
    : AddressSpace      (storage),
    m_interruptManager  (im),
    m_speedMode         (sm),
    m_div               (0),
    m_tac               (0),
    m_tma               (0),
    m_tima              (0),
    m_prevBit           (false),
    m_overflow          (false),
    m_ticksSinceOverflow(0)
{}

//public
void Timer::tick()
{
    updateDiv((m_div + 1) & 0xffff);

    if (m_overflow)
    {
        m_ticksSinceOverflow++;

        if (m_ticksSinceOverflow == 4)
        {
            m_interruptManager.requestInterrupt(Interrupt::Timer);
        }
        else if (m_ticksSinceOverflow == 5)
        {
            m_tima = m_tma;
        }
        else if (m_ticksSinceOverflow == 6)
        {
            m_tima = m_tma;
            m_overflow = false;
            m_ticksSinceOverflow = 0;
        }
    }
        
}

bool Timer::accepts(std::uint16_t address) const
{
    return (address >= 0xff04 && address <= 0xff07);
}

void Timer::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    switch (address)
    {
    default: break;
    case 0xff04:
        updateDiv(0);
        break;
    case 0xff05:
        if (m_ticksSinceOverflow < 5)
        {
            m_tima = value;
            m_overflow = false;
            m_ticksSinceOverflow = 0;
        }
        break;
    case 0xff06:
        m_tma = value;
        break;
    case 0xff07:
        m_tac = value;
        break;
    }
}

std::uint8_t Timer::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    switch (address)
    {
    default: break;
    case 0xff04:
        return static_cast<std::uint8_t>(m_div >> 8);
        break;
    case 0xff05:
        return static_cast<std::uint8_t>(m_tima);
    case 0xff06:
        return static_cast<std::uint8_t>(m_tma);
    case 0xff07:
        return static_cast<std::uint8_t>(m_tac);
    }
    return 0;
}

//private
void Timer::incTma()
{
    m_tima++;
    m_tima %= 0x100;
    if (m_tima == 0)
    {
        m_overflow = true;
        m_ticksSinceOverflow = 0;
    }
}

void Timer::updateDiv(std::uint16_t newDiv)
{
    m_div = newDiv;
    auto bitPos = FreqToBit[m_tac & 0b11];
    bitPos <<= m_speedMode.getSpeedMode() - 1;

    bool bit = ((m_div & (1 << bitPos)) != 0);
    bit &= ((m_tac & (1 << 2)) != 0);

    if (!bit && m_prevBit)
    {
        incTma();
    }
    m_prevBit = bit;
}