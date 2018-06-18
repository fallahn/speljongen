#include "AudioChannelOne.hpp"

ChannelOne::ChannelOne(std::vector<std::uint8_t>& storage)
    : AudioChannel(storage, 0xff10, 64),
    m_frequencyDivider  (0),
    m_lastOutput        (0),
    m_index             (0)
{

}

//public
void ChannelOne::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    
    AudioChannel::setByte(address, value);
    auto reg = address - getOffset();

    switch (reg)
    {
    default: break;
    case 0:
        m_frequencySweep.setNr10(value);
        break;
    case 1:
        getSampleCounter().setLength(64 - (value & 0x3f));
        break;
    case 2:
        m_volumeEnvelope.setNr2(value);
        setDacEnabled((value & 0xf8) != 0);
        setEnabled(getEnabled() & getDacEnabled());
        break;
    case 3:
        m_frequencySweep.setNr13(value);
        break;
    case 4:
        m_frequencySweep.setNr14(value);
        break;
    }
}

std::uint8_t ChannelOne::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    auto reg = address - getOffset();
    if (reg == 3)
    {
        return m_frequencySweep.getNr13();
    }

    if (reg == 4)
    {
        return (AudioChannel::getByte(address) & 0xf8) | (m_frequencySweep.getNr14() & 0x7);
    }

    return AudioChannel::getByte(address);
}

void ChannelOne::start()
{
    m_index = 0;
    if (isColour())
    {
        getSampleCounter().reset();
    }

    getSampleCounter().start();
    m_frequencySweep.start();
    m_volumeEnvelope.start();
}

void ChannelOne::trigger()
{
    m_index = 0;
    m_frequencyDivider = 1;
    m_volumeEnvelope.trigger();
}

std::int32_t ChannelOne::tick()
{
    bool b = true;
    b = updateCounter() && b;
    b = updateSweep() && b;
    b = getDacEnabled() && b;
    if (!b)
    {
        return 0;
    }

    if (--m_frequencyDivider == 0)
    {
        resetFreqDivider();
        m_lastOutput = (getDuty() & (1 << m_index)) >> m_index;
        m_index = (m_index + 1) % 8;
    }

    return m_lastOutput * m_volumeEnvelope.getVolume();
}

//private
std::int32_t ChannelOne::getDuty() const
{
    auto val = AudioChannel::getByte(getOffset() + 1) >> 6;
    switch (val)
    {
    case 0:
        return 0x1;
    case 1:
        return 0x81;
    case 2:
        return 0x87;
    case 3:
        return 0x7e;
    default: assert(false); return -1;
    }
}

void ChannelOne::resetFreqDivider()
{
    m_frequencyDivider = getFrequency() * 4;
}

bool ChannelOne::updateSweep()
{
    m_frequencySweep.tick();
    if (getEnabled() && !m_frequencySweep.enabled())
    {
        setEnabled(false);
    }
    return getEnabled();
}