#pragma once

#include "AddressSpace.hpp"
#include "SpeedMode.hpp"

#include <cassert>

class Dma final : public AddressSpace
{
public:
    Dma(AddressSpace& oam, const SpeedMode& speedMode)
        : m_oam(oam), m_speedMode(speedMode),
        m_inProgress(false), m_restarted(false), m_from(0), m_ticks(0)
    {

    }

    AddressSpace::Type type() const override { return AddressSpace::Dma; }

    bool accepts(std::uint16_t address) const override
    {
        return address == 0xff46;
    }

    std::uint8_t getByte(std::uint16_t) const override { return 0; }

    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        assert(accepts(address);)
        m_from = value * 0x100;
        m_restarted = isOamBlocked();
        m_ticks = 0;
        m_inProgress = true;
    }

    bool isOamBlocked() const
    {
        return m_restarted || (m_inProgress && m_ticks > 4);
    }

    void tick()
    {
        if (m_inProgress)
        {
            if (++m_ticks >= 648 / m_speedMode.getSpeedMode())
            {
                m_inProgress = false;
                m_restarted = false;
                m_ticks = 0;

                //do transfer
                for (std::uint16_t i = 0; i < 0xa0; ++i)
                {
                    m_oam.setByte(0xfe00 + i, getStorage()[m_from + i]);
                }
            }
        }
    }

private:
    AddressSpace& m_oam;
    const SpeedMode& m_speedMode;

    bool m_inProgress;
    bool m_restarted;

    std::uint16_t m_from;
    std::uint32_t m_ticks;
};