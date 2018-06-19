#include "AudioOutput.hpp"
#include "Consts.hpp"

#include <SFML/System/Lock.hpp>

#include <limits>
#include <iostream>

namespace
{
    const std::uint32_t SampleRate = 22050;
    const std::uint32_t TickRate = CYCLES_PER_SECOND / SampleRate;
    const std::size_t BufferSize = SampleRate * 4;
}

AudioOutput::AudioOutput()
    : m_bufferSize  (0),
    m_tick          (0)
{
    m_buffer.resize(BufferSize);
    m_outBuffer.resize(BufferSize);
    m_waveformBufferL.reserve(BufferSize / 2);
    m_waveformBufferR.reserve(BufferSize / 2);
    
    for (auto i = 0; i < /*BufferSize*/388; ++i)
    {
        m_emptyBuffer.push_back(0);
    }

    initialize(2, SampleRate);
}

//public
void AudioOutput::addSample(std::uint8_t left, std::uint8_t right)
{
    if (m_tick++ == 0 && m_bufferSize < BufferSize - 2)
    {
        //we have to normalise as float then convert to 16bit...
        sf::Int16 l = to16Bit(left, m_waveformBufferL);
        sf::Int16 r = to16Bit(right, m_waveformBufferR);

        sf::Lock lock(m_mutex);
        m_buffer[m_bufferSize++] = l;
        m_buffer[m_bufferSize++] = r;
    }
    else
    {
        m_tick %= TickRate;
    }
}

//private
sf::Int16 AudioOutput::to16Bit(std::uint8_t value, std::vector<float>& waveformBuffer)
{
    float f = static_cast<float>(value) / 255.f;
    f *= 2.f;
    f -= 1.f;
    //waveformBuffer[m_bufferSize / 2] = f;

    f *= static_cast<float>(std::numeric_limits<sf::Int16>::max());

    return static_cast<sf::Int16>(f);
}

bool AudioOutput::onGetData(Chunk& chunk)
{
    if (m_bufferSize)
    {
        m_mutex.lock();
        chunk.sampleCount = m_bufferSize;
        m_bufferSize = 0;
        m_buffer.swap(m_outBuffer);
        m_mutex.unlock();

        chunk.samples = m_outBuffer.data();
        return true;
    }
    chunk.sampleCount = m_emptyBuffer.size();
    chunk.samples = m_emptyBuffer.data();
    return true;
}