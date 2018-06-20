#include "AudioChannelTwo.hpp"

ChannelTwo::ChannelTwo(std::vector<std::uint8_t>& storage)
    : AudioChannel(storage, 0xff15, 64),
    m_frequencyDivider  (0),
    m_lastOutput        (0),
    m_index             (0)
{

}

//public
void ChannelTwo::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));

    AudioChannel::setByte(address, value);
    auto reg = address - getOffset();

    switch (reg)
    {
    default:break;
    case 1:
        getSampleCounter().setLength(64 - (value & 0x3f));
        break;
    case 2:
        m_volumeEnvelope.setNr2(value);
        setDacEnabled((value & 0xf8) != 0);
        setEnabled(getEnabled() & getDacEnabled());
        break;
    }
}

std::uint8_t ChannelTwo::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    return AudioChannel::getByte(address);
}

void ChannelTwo::start()
{
    m_index = 0;
    if (isColour())
    {
        getSampleCounter().reset();
    }
    getSampleCounter().start();
    m_volumeEnvelope.start();
}

void ChannelTwo::trigger()
{
    m_index = 0;
    m_frequencyDivider = 1;
    m_volumeEnvelope.trigger();
}

std::int32_t ChannelTwo::tick()
{
    m_volumeEnvelope.tick();

    bool b = true;
    b = updateCounter() && b;
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

std::int32_t ChannelTwo::getDuty() const
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

void ChannelTwo::resetFreqDivider()
{
    m_frequencyDivider = getFrequency() * 4;
}