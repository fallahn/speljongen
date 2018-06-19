#include "AudioOutput.hpp"
#include "Consts.hpp"

#include <SFML/System/Lock.hpp>
#include <SFML/System/Clock.hpp>

#include <limits>
#include <iostream>
#include <array>

namespace
{
    const std::uint32_t SampleRate = 22050;
    //const std::uint32_t TickRate = CYCLES_PER_SECOND / SampleRate;
    const std::size_t BufferSize = SampleRate * 4;
    const std::array<std::int16_t, 2> buns = {};
}

AudioOutput::AudioOutput()
    : m_left        (0),
    m_right         (0),
    m_bufferSize    (0),
    m_waveformIndex (0),
    m_threadRunning (false),
    m_thread        (&AudioOutput::threadFunc, this)
    //m_tick          (0)
{
    m_buffer.resize(BufferSize);
    m_outBuffer.resize(BufferSize);
    m_waveformBufferL.resize(BufferSize / 2);
    m_waveformBufferR.resize(BufferSize / 2);
    
    //for (auto i = 0; i < /*BufferSize*/388; ++i)
    //{
    //    m_emptyBuffer.push_back(0);
    //}

    initialize(2, SampleRate);

    m_thread.launch();
}

AudioOutput::~AudioOutput()
{
    if (m_threadRunning)
    {
        m_threadRunning = false;
        m_thread.wait();
    }
}

//public
void AudioOutput::addSample(std::uint8_t left, std::uint8_t right)
{
    //if (m_tick++ == 0 && m_bufferSize < BufferSize - 2)
    //{
    //    //we have to normalise as float then convert to 16bit...
    //    sf::Int16 l = to16Bit(left, m_waveformBufferL);
    //    sf::Int16 r = to16Bit(right, m_waveformBufferR);

    //    sf::Lock lock(m_mutex);
    //    m_buffer[m_bufferSize++] = l;
    //    m_buffer[m_bufferSize++] = r;
    //}
    //else
    //{
    //    m_tick %= TickRate;
    //}

    m_left = to16Bit(left, m_waveformBufferL);
    m_right = to16Bit(right, m_waveformBufferR);
    m_waveformIndex = (m_waveformIndex + 1) % (BufferSize / 2);
}

//private
void AudioOutput::threadFunc()
{
    //OK let's see if we can run this at 22khz :)
    static const sf::Time frameTime = sf::milliseconds(1000) / 44100000.f;
    sf::Time accumulator;
    sf::Clock timer;

    m_threadRunning = true;
    while (m_threadRunning)
    {
        accumulator += timer.restart();
        std::int32_t maxSamples = 10;
        while (accumulator > frameTime && maxSamples--)
        {
            accumulator -= frameTime;

            sf::Lock lock(m_mutex);

            //for (auto i = 0; i < 10; ++i)
            {
                m_buffer[m_bufferSize] = m_left;
                m_bufferSize = (m_bufferSize + 1) % BufferSize;
                m_buffer[m_bufferSize] = m_right;
                m_bufferSize = (m_bufferSize + 1) % BufferSize;
            }
            //std::cout << m_bufferSize << "\n";
        }
        //if (m_bufferSize) play();
    }
}

sf::Int16 AudioOutput::to16Bit(std::uint8_t value, std::vector<float>& waveformBuffer)
{
    float f = static_cast<float>(value) / 255.f;
    f *= 2.f;
    f -= 1.f;
    waveformBuffer[m_waveformIndex] = f;

    f *= static_cast<float>(std::numeric_limits<sf::Int16>::max());

    return static_cast<sf::Int16>(f);
}

bool AudioOutput::onGetData(Chunk& chunk)
{
    //std::cout << "get data" << "\n";
    /*chunk.sampleCount = 2;
    chunk.samples = buns.data();*/

    sf::Lock lock(m_mutex);
    if (m_bufferSize)
    {

        chunk.sampleCount = m_bufferSize;
        chunk.samples = m_buffer.data();
        m_bufferSize = 0;
        //m_outBuffer.swap(m_buffer);
        m_mutex.unlock();
        //chunk.samples = m_outBuffer.data();
        return true;
    }
    //chunk.sampleCount = m_emptyBuffer.size();
    //chunk.samples = m_emptyBuffer.data();
    return false;
}