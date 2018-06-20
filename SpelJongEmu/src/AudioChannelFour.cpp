#include "AudioChannelFour.hpp"

ChannelFour::ChannelFour(std::vector<std::uint8_t>& storage)
    : AudioChannel(storage, 0xff1f, 64)
{

}

//public
void ChannelFour::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    AudioChannel::setByte(address, value);

    auto reg = address - getOffset();
    switch (reg)
    {
    default: break;
    case 1:
        getSampleCounter().setLength(64 - (value & 0x3f));
        break;
    case 2:
        m_volumeEnvelope.setNr2(value);
        setDacEnabled((value & 0xf8) != 0);
        setEnabled(getEnabled() & getDacEnabled());
        break;
    case 3:
        m_counter.setNr43(value);
        break;
    }
}

std::uint8_t ChannelFour::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    return AudioChannel::getByte(address);
}

void ChannelFour::start()
{
    if (isColour())
    {
        getSampleCounter().reset();
    }
    getSampleCounter().start();
    m_lfsr.start();
    m_volumeEnvelope.start();
}

void ChannelFour::trigger()
{
    m_lfsr.reset();
    m_volumeEnvelope.trigger();
}

std::int32_t ChannelFour::tick()
{
    m_volumeEnvelope.tick();

    if (!updateCounter())
    {
        return 0;
    }

    if (!getDacEnabled())
    {
        return 0;
    }

    if (m_counter.tick())
    {
        m_lastResult = m_lfsr.nextBit((getStorageValue(0xff22) & (1 << 3)) != 0);
    }

    return m_lastResult * m_volumeEnvelope.getVolume();
}