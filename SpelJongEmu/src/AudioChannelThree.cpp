#include "AudioChannelThree.hpp"

#include <array>

namespace
{

}

ChannelThree::ChannelThree(std::vector<std::uint8_t>& storage)
    : AudioChannel(storage, 0xff1a, 256)
{

}

//public
bool ChannelThree::accepts(std::uint16_t address) const
{
    return AudioChannel::accepts(address)
        || (address >= 0xff30 && address < 0xff40);
}

void ChannelThree::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    
    //write wave ram
    if (address >= 0xff30 && address < 0xff40)
    {
        if (!isEnabled())
        {
            setStorageValue(address, value);
        }
        else if((m_lastReadAddress >= 0xff30 && m_lastReadAddress < 0xff40)
            &&(isColour() || m_ticksSinceLastRead < 2))
        {
            setStorageValue(m_lastReadAddress, value);
        }
        return;
    }

    //else write registers
    AudioChannel::setByte(address, value);
    auto reg = address - getOffset();
    switch (reg)
    {
    default: break;
    case 0:
        setDacEnabled((value & (1 << 7)) != 0);
        setEnabled(getEnabled() & getDacEnabled());
        break;
    case 1:
        getSampleCounter().setLength(256 - value);
        break;
    case 4:
        if (!isColour() && (value & (1 << 7)) != 0)
        {
            if (isEnabled() && m_frequencyDivider == 2)
            {
                auto position = m_index / 2;
                if (position < 4)
                {
                    setStorageValue(0xff30, getStorageValue(0xff30 + position));
                }
                else
                {
                    position &= ~3;
                    for (auto i = 0; i < 4; ++i)
                    {
                        setStorageValue(0xff30 + i, getStorageValue(0xff30 + ((position + i) % 0x10)));
                    }
                }
            }
        }
        break;
    }
}

std::uint8_t ChannelThree::getByte(std::uint16_t address) const
{
    assert(accepts(address));

    //read wave ram
    if (address >= 0xff30 && address < 0xff40)
    {
        if (!isEnabled())
        {
            return getStorageValue(address);
        }

        if ((m_lastReadAddress >= 0xff30 && m_lastReadAddress < 0xff40)
            && (isColour() || m_ticksSinceLastRead < 2))
        {
            return getStorageValue(m_lastReadAddress);
        }

        return 0xff;
    }

    //else read registers
    return AudioChannel::getByte(address);
}

void ChannelThree::start()
{
    m_index = 0;
    m_buffer = 0;
    if (isColour())
    {
        getSampleCounter().reset();
    }
    getSampleCounter().start();
}

void ChannelThree::trigger()
{
    m_index = 0;
    m_frequencyDivider = 6;
    m_triggered = !isColour();

    if (isColour())
    {
        getWaveEntry();
    }
}

std::int32_t ChannelThree::tick()
{
    m_ticksSinceLastRead++;

    if (!updateCounter())
    {
        return 0;
    }

    if (!getDacEnabled())
    {
        return 0;
    }

    if ((AudioChannel::getByte(0xff1a) & (1 << 7)) == 0)
    {
        return 0;
    }

    if (--m_frequencyDivider == 0)
    {
        resetFreqDivider();
        if (m_triggered)
        {
            m_lastOutput = (m_buffer >> 4) & 0x0f;
            m_triggered = false;
        }
        else
        {
            m_lastOutput = getWaveEntry();
        }
        m_index = (m_index + 1) % 32;
    }
    return m_lastOutput;
}

//private
std::uint8_t ChannelThree::getVolume() const
{
    return (AudioChannel::getByte(0xff1c) >> 5) & 0x3;
}

std::uint8_t ChannelThree::getWaveEntry()
{
    m_ticksSinceLastRead = 0;
    m_lastReadAddress = 0xff30 + (m_index / 2);
    m_buffer = getStorageValue(m_lastReadAddress);

    auto b = m_buffer;
    if (m_index % 2 == 0)
    {
        b >>= 4;
        b &= 0x0f;
    }
    else
    {
        b &= 0x0f;
    }

    switch (getVolume())
    {
    default: assert(false); break;
    case 0: return 0;
    case 1: return b;
    case 2: return b >> 1;
    case 3: return b >> 2;
    }
    return 0;
}

void ChannelThree::resetFreqDivider()
{
    m_frequencyDivider = getFrequency() * 2;
}