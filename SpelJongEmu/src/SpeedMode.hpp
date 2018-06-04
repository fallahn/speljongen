#pragma once

#include "AddressSpace.hpp"

class SpeedMode final : public AddressSpace
{
public:
    explicit SpeedMode(std::vector<std::uint8_t>&);

    std::string getLabel() const override { return "Speed Mode"; }

    bool accepts(std::uint16_t) const override;
    void setByte(std::uint16_t, std::uint8_t) override;
    std::uint8_t getByte(std::uint16_t) const override;

    bool onStop();
    std::uint32_t getSpeedMode() const;

private:
    bool m_currentSpeed;
    bool m_prepareSpeedSwitch;
};
