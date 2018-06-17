#include "SpeedMode.hpp"

SpeedMode::SpeedMode(std::vector<std::uint8_t>& storage)
    : AddressSpace      (storage),
    m_currentSpeed      (false),
    m_prepareSpeedSwitch(false)
{

}

//public
bool SpeedMode::accepts(std::uint16_t address) const
{
    return address == 0xff4d;
}

void SpeedMode::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    m_prepareSpeedSwitch = ((value & 0x01) != 0);
    std::uint8_t data = (m_currentSpeed ? (1 << 7) : 0) | (m_prepareSpeedSwitch ? (1 << 0) : 0) | 0b01111110;
    setStorageValue(address, data);
}

std::uint8_t SpeedMode::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    //return (m_currentSpeed ? (1 << 7) : 0) | (m_prepareSpeedSwitch ? (1 << 0) : 0) | 0b01111110;
    return getStorageValue(address);
}

bool SpeedMode::onStop()
{
    if (m_prepareSpeedSwitch)
    {
        m_currentSpeed = !m_currentSpeed;
        m_prepareSpeedSwitch = false;

        std::uint8_t data = (m_currentSpeed ? (1 << 7) : 0) | (m_prepareSpeedSwitch ? (1 << 0) : 0) | 0b01111110;
        setStorageValue(0xff4d, data);

        return true;
    }
    return false;
}

std::uint32_t SpeedMode::getSpeedMode() const
{
    return m_currentSpeed ? 2 : 1;
}