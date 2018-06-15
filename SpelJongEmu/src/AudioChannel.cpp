#include "AudioChannel.hpp"

AudioChannel::AudioChannel(std::vector<std::uint8_t>& storage, std::uint16_t offset, std::uint32_t length)
    : AddressSpace  (storage),
    m_offset        (offset),
    m_colour        (false),
    m_enabled       (false),
    m_dacEnabled    (false),
    m_sampleCounter (length)
{

}

//public
bool AudioChannel::isEnabled() const
{
    return m_enabled && m_dacEnabled;
}

bool AudioChannel::accepts(std::uint16_t address) const
{
    return address >= m_offset && address < (m_offset + 5);
}

void AudioChannel::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    setStorageValue(address, value);

    if (address - m_offset == 4)
    {
        m_sampleCounter.setNr4(value);
        if ((value & (1 << 7)) != 0)
        {
            m_enabled = m_dacEnabled;
            trigger();
        }
    }    
}

std::uint8_t AudioChannel::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    return getStorageValue(address);
}

void AudioChannel::stop()
{
    m_enabled = false;
}

//protected
std::uint16_t AudioChannel::getFrequency() const
{
    return 2048 - (getStorageValue(m_offset + 3) | ((getStorageValue(m_offset + 4) & 0b111) << 8));
}

bool AudioChannel::updateCounter()
{
    m_sampleCounter.tick();
    if (m_sampleCounter.isEnabled())
    {
        return m_enabled;
    }

    if (m_enabled && m_sampleCounter.getLength() == 0)
    {
        m_enabled = false;
    }

    return m_enabled;
}