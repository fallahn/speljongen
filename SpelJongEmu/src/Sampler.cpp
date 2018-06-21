#include "Sampler.hpp"

Sampler::Sampler(double inFreq, double outFreq, std::size_t buffSize)
{
    reset(inFreq, outFreq, buffSize);
}

void Sampler::write(double sample)
{
    m_history[0] = m_history[1];
    m_history[1] = m_history[2];
    m_history[2] = m_history[3];
    m_history[3] = sample;

    while (m_fraction <= 1.0)
    {
        auto a = m_history[3] - m_history[2] - m_history[0] + m_history[1];
        auto b = m_history[0] - m_history[1] - a;
        auto c = m_history[2] - m_history[0];
        auto d = m_history[1];

        m_buffer.write(a * m_fraction * m_fraction * m_fraction + b * m_fraction * m_fraction + c * m_fraction + d);
        m_fraction += m_ratio;
    }
    m_fraction -= 1.0;
}

void Sampler::reset(double inputFreq, double outputFreq, std::size_t size)
{
    m_inputFreq = inputFreq;
    m_outputFreq = outputFreq;

    if (size == 0)
    {
        size = static_cast<std::size_t>(outputFreq * 0.02);
    }

    m_ratio = m_inputFreq / m_outputFreq;
    m_fraction = 0.0;
    m_history = {};
    m_buffer.resize(size, 0.0);
}