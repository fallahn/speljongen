#include "AudioChannel.hpp"

SampleCounter::SampleCounter(std::uint32_t length)
    : m_fullLength  (length),
    m_length        (0),
    m_index         (0),
    m_enabled       (false)
{

}

//public
void SampleCounter::start()
{
    m_index = 8192;
}

void SampleCounter::tick()
{
    if (++m_index == divisor)
    {
        m_index = 0;
        if (m_enabled && m_length > 0)
        {
            m_length--;
        }
    }
}

void SampleCounter::setLength(std::uint32_t length)
{
    m_length = length > 0 ? length : m_fullLength;
}

void SampleCounter::setNr4(std::uint8_t value)
{
    bool enable = (value & (1 << 6)) != 0;
    bool trigger = (value & (1 << 7)) != 0;

    if (m_enabled)
    {
        if (m_length == 0 && trigger)
        {
            if (enable && m_index < (divisor / 2))
            {
                setLength(m_fullLength - 1);
            }
            else
            {
                setLength(m_fullLength);
            }
        }
    }
    else if (enable)
    {
        if (m_length > 0 && m_index < (divisor / 2))
        {
            m_length--;
        }
        
        if (m_length == 0 && trigger && m_index < (divisor / 2))
        {
            setLength(m_fullLength - 1);
        }
    }
    else
    {
        if (m_length == 0 && trigger)
        {
            setLength(m_fullLength);
        }
    }
    m_enabled = enable;
}

std::uint32_t SampleCounter::getLength() const
{
    return m_length;
}

bool SampleCounter::isEnabled() const
{
    return m_enabled;
}

void SampleCounter::reset()
{
    m_enabled = true;
    m_index = 0;
    m_length = 0;
}