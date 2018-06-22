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
    const std::uint32_t SampleRate = 48000;
    const std::size_t BufferSize = 1024;
    const std::array<std::int16_t, 2000> buns = {};
    

    const std::uint32_t TickCount = (CYCLES_PER_SECOND / SampleRate);
    const std::uint32_t TickMod = (CYCLES_PER_SECOND % SampleRate);
    const std::size_t WaveformSize = (BufferSize * TickCount) / 2;

    SDL_AudioDeviceID audioDevice = 0;
}

AudioOutput::AudioOutput()
    : m_bufferSize  (0),
    m_waveformIndex (0),
    m_tick          (0),
    m_samplerL      (CYCLES_PER_SECOND, SampleRate, 800),
    m_samplerR      (CYCLES_PER_SECOND, SampleRate, 800),
    m_running       (false)
{
    m_buffer.resize(BufferSize);
    m_waveformBufferL.resize(WaveformSize);
    m_waveformBufferR.resize(WaveformSize);
    

    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.channels = 2;
    spec.format = AUDIO_S16;
    spec.freq = SampleRate;
    spec.samples = BufferSize / 2; //pairs of shorts!

    SDL_AudioSpec got;

    audioDevice = SDL_OpenAudioDevice(nullptr, 0, &spec, &got, 0);
    if (audioDevice == 0)
    {
        std::cout << "Failed to open audio device :(\n";
    }
    //else
    //{
    //    //SDL_PauseAudioDevice(audioDevice, 0);
    //}
}

AudioOutput::~AudioOutput()
{
    if (audioDevice)
    {
        SDL_CloseAudioDevice(audioDevice);
    }

    SDL_Quit();
}

//public
void AudioOutput::addSample(std::uint8_t left, std::uint8_t right)
{
    double f = std::min(1.0, static_cast<double>(left) / 255.0);
    f *= 2.0 - 1.0;
    m_waveformBufferL[m_waveformIndex] = static_cast<float>(f) * (255.f / 16.f);
    m_samplerL.write(f);

    f = std::min(1.0, static_cast<double>(right) / 255.0);
    f *= 2.0 - 1.0;
    m_waveformBufferR[m_waveformIndex] = static_cast<float>(f) * (255.f / 16.f);
    m_samplerR.write(f);
    m_waveformIndex = (m_waveformIndex + 1) % WaveformSize;

    m_tick++;

    static std::uint32_t buffered = 0;
    static std::uint32_t skip = 0;
    static std::uint32_t skipCount = 0;

    if (m_samplerL.pending())
    {
        std::int16_t left = static_cast<std::int16_t>(m_samplerL.read() * static_cast<double>(std::numeric_limits<std::int16_t>::max()));
        std::int16_t right = static_cast<std::int16_t>(m_samplerR.read() * static_cast<double>(std::numeric_limits<std::int16_t>::max()));

        if (skip)
        {
            skipCount = (skipCount + 1) % skip;
        }


        if (skip == 0 || skipCount == skip)
        {
            m_buffer[m_bufferSize] = left;
            m_buffer[m_bufferSize + 1] = right;
            m_bufferSize = (m_bufferSize + 2) % BufferSize;
        }
        else
        {
            buffered--;
        }


        if (m_tick > TickMod && skip == 0)
        {
            buffered =  SDL_GetQueuedAudioSize(audioDevice);
            m_tick %= TickMod;
        }
        //limit the amount which is buffered and drop a frame of audio if needs be
        if (buffered > (BufferSize * 30))
        {
            //TODO make this a moving average
            skip = 2; //nth frame is added
            //std::cout << "skip on\n";
        }
        else
        {
            skip = 0;
            skipCount = 0;
        }


        if (m_bufferSize == 0 && audioDevice)
        {
            SDL_QueueAudio(audioDevice, m_buffer.data(), static_cast<Uint32>(BufferSize) * sizeof(std::int16_t));
        }
    }
}

std::size_t AudioOutput::getWaveformSize() const { return WaveformSize; }

void AudioOutput::start()
{
    m_tick = 0;
    m_bufferSize = 0;

    if (audioDevice)
    {
        SDL_PauseAudioDevice(audioDevice, 0);
        SDL_ClearQueuedAudio(audioDevice);
        m_running = true;
    }
}

void AudioOutput::stop()
{
    if (audioDevice)
    {
        SDL_PauseAudioDevice(audioDevice, 1);
        m_running = false;
    }
}

//private