#include "AudioSweep.hpp"

AudioSweep::AudioSweep()
    : m_period(0),
    m_negate            (false),
    m_shift             (0),
    m_timer             (0),
    m_shadowFrequency   (0),
    m_nr13              (0),
    m_nr14              (0),
    m_index             (0),
    m_overflow          (false),
    m_counterEnabled    (false),
    m_negated           (false)
{

}

//public
void AudioSweep::start()
{
    m_counterEnabled = false;
    m_index = 8192;
}

void AudioSweep::trigger()
{
    m_negated = false;
    m_overflow = false;
    m_shadowFrequency = m_nr13 | ((m_nr14 & 0x7) << 8);
    m_timer = m_period == 0 ? 8 : m_period;
    m_counterEnabled = (m_period != 0 || m_shift != 0);

    if (m_shift > 0)
    {
        calculate();
    }
}

void AudioSweep::setNr10(std::uint8_t value)
{
    m_period = (value >> 4) & 0x7;
    m_negate = (value & (1 << 3)) != 0;
    m_shift = value & 0x7;

    if (m_negated && !m_negate)
    {
        m_overflow = true;
    }
}

void AudioSweep::setNr13(std::uint8_t value)
{
    m_nr13 = value;
}

void AudioSweep::setNr14(std::uint8_t value)
{
    m_nr14 = value;
    if ((value & (1 << 7)) != 0)
    {
        trigger();
    }
}

std::uint8_t AudioSweep::getNr13() const
{
    return m_nr13;
}

std::uint8_t AudioSweep::getNr14() const
{
    return m_nr14;
}

void AudioSweep::tick()
{
    if (++m_index == divisor)
    {
        m_index = 0;
        if (!m_counterEnabled)
        {
            return;
        }

        if (--m_timer == 0)
        {
            m_timer = (m_period == 0) ? 8 : m_period;

            if (m_period != 0)
            {
                auto newFrequency = calculate();
                if (!m_overflow && m_shift != 0)
                {
                    m_shadowFrequency = newFrequency;
                    m_nr13 = static_cast<std::uint8_t>(m_shadowFrequency & 0xff);
                    m_nr14 = static_cast<std::uint8_t>((m_shadowFrequency & 0x700) >> 8);
                    calculate();
                }
            }
        }
    }
}

bool AudioSweep::enabled() const
{
    return !m_overflow;
}

//private
std::int32_t AudioSweep::calculate()
{
    auto freq = m_shadowFrequency >> m_shift;
    if (m_negate)
    {
        freq = m_shadowFrequency - freq;
        m_negated = true;
    }
    else
    {
        freq = m_shadowFrequency + freq;
    }

    if (freq > 2047)
    {
        m_overflow = true;
    }
    return freq;
}