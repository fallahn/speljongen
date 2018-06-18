#include "AudioEnvelope.hpp"
#include "Consts.hpp"

VolumeEnvelope::VolumeEnvelope()
    : m_startVolume     (0),
    m_envelopeDirection (0),
    m_sweep             (0),
    m_volume            (0),
    m_index             (0),
    m_finished          (false)
{

}

//public
void VolumeEnvelope::setNr2(std::uint8_t value)
{
    m_startVolume = value >> 4;
    m_envelopeDirection = (value & (1 << 3)) == 0 ? -1 : 1;
    m_sweep = value & 0x7;
}

bool VolumeEnvelope::enabled() const
{
    return m_sweep > 0;
}

void VolumeEnvelope::start()
{
    m_finished = true;
    m_index = 8192;
}

void VolumeEnvelope::trigger()
{
    m_volume = m_startVolume;
    m_index = 0;
    m_finished = false;
}

void VolumeEnvelope::tick()
{
    if (m_finished) return;

    if ((m_volume == 0 && m_envelopeDirection == -1)
        || (m_volume == 15 && m_envelopeDirection == 1))
    {
        m_finished = true;
        return;
    }

    static const std::int32_t sweepMultiplier = CYCLES_PER_SECOND / 64;
    if (++m_index == m_sweep * sweepMultiplier)
    {
        m_index = 0;
        m_volume += m_envelopeDirection;
    }
}

std::int32_t VolumeEnvelope::getVolume() const
{
    return m_sweep > 0 ? m_volume : m_startVolume;
}