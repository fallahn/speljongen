#include "AudioOutput.hpp"
#include "Consts.hpp"

#include <SFML/System/Lock.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>

#include <SDL.h>

#include <limits>
#include <iostream>
#include <array>

namespace
{
    const std::uint32_t SampleRate = 22050;
    const std::size_t BufferSize = 256;// SampleRate * 4;
    const std::array<std::int16_t, 2000> buns = {};

    static const std::uint32_t TickCount = (CYCLES_PER_SECOND / SampleRate) + 1;

    SDL_AudioDeviceID audioDevice = 0;
}

AudioOutput::AudioOutput()
    : m_left        (0),
    m_right         (0),
    m_bufferSize    (0),
    m_waveformIndex (0),
    m_threadRunning (false),
    m_thread        (&AudioOutput::threadFunc, this),
    m_tick          (0)
{
    m_buffer.resize(BufferSize);
    //m_outBuffer.resize(BufferSize);
    m_waveformBufferL.resize(BufferSize / 2);
    m_waveformBufferR.resize(BufferSize / 2);
    

    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.channels = 2;
    spec.format = AUDIO_S16;
    spec.freq = 22050;
    spec.samples = BufferSize / 2; //pairs of shorts!

    SDL_AudioSpec got;

    audioDevice = SDL_OpenAudioDevice(nullptr, 0, &spec, &got, 0);
    if (audioDevice == 0)
    {
        std::cout << "Failed to open audio device :(\n";
    }
    else
    {
        SDL_PauseAudioDevice(audioDevice, 0);
    }

    //m_thread.launch(); //don't use this.
}

AudioOutput::~AudioOutput()
{
    if (m_threadRunning)
    {
        m_threadRunning = false;
        m_thread.wait();
    }

    if (audioDevice)
    {
        SDL_CloseAudioDevice(audioDevice);
    }

    SDL_Quit();
}

//public
void AudioOutput::addSample(std::uint8_t left, std::uint8_t right)
{
    m_left = to16Bit(left, m_waveformBufferL);
    m_right = to16Bit(right, m_waveformBufferR);
    m_waveformIndex = (m_waveformIndex + 1) % (BufferSize / 2);

    if (m_tick++ == 0)
    {
        m_buffer[m_bufferSize] = m_left;
        m_buffer[m_bufferSize + 1] = m_right;
        m_bufferSize = (m_bufferSize + 2) % BufferSize;

        if (m_bufferSize == 0) //wrapped around
        {
            int ret = SDL_QueueAudio(audioDevice, m_buffer.data(), BufferSize * 2);
        }
    }
    else
    {
        m_tick %= TickCount;
    }
}

//private
void AudioOutput::threadFunc()
{
    //OK let's see if we can run this at 22khz :)
    static const sf::Time frameTime = sf::microseconds(46);// sf::milliseconds(1000) / 2050000.f;
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
                m_buffer[m_bufferSize + 1] = m_right;
                m_bufferSize = (m_bufferSize + 2) % BufferSize;
            }
        }

        if (m_bufferSize == 0) //wrapped around
        {
            int ret = SDL_QueueAudio(audioDevice, m_buffer.data(), BufferSize * 2);
        }
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