#pragma once

#include "AddressSpace.hpp"
#include "SpeedMode.hpp"

#include <cassert>

class Dma final : public AddressSpace
{
public:
    Dma(std::vector<std::uint8_t>& storage, const SpeedMode& speedMode)
        : AddressSpace(storage), m_speedMode(speedMode),
        m_inProgress(false), m_restarted(false), m_ticks(0)
    {
        setStorageValue(0xff46, 0);
    }

    AddressSpace::Type type() const override { return AddressSpace::Type::Dma; }

    bool accepts(std::uint16_t address) const override
    {
        return address == 0xff46;
    }

    std::uint8_t getByte(std::uint16_t address) const override 
    { 
        assert(accepts(address));
        return getStorageValue(address);
    }

    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        assert(accepts(address));
        setStorageValue(address, value);
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
            if (++m_ticks >= (648 / m_speedMode.getSpeedMode()))
            {
                m_inProgress = false;
                m_restarted = false;
                m_ticks = 0;

                //do transfer
                std::uint16_t from = getStorageValue(0xff46) * 0x100;
                for (std::uint16_t i = 0; i < 0xa0; ++i)
                {
                    setStorageValue(0xfe00 + i, getStorageValue(from + i));
                }
            }
        }
    }

private:
    const SpeedMode& m_speedMode;

    bool m_inProgress;
    bool m_restarted;

    std::uint32_t m_ticks;
};