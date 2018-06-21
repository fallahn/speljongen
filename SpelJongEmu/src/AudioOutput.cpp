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
    m_waveformBufferL[m_waveformIndex] = static_cast<float>(f) * (255.f / 16.f);
    m_samplerL.write(f);

    f = std::min(1.0, static_cast<double>(right) / 255.0);
    m_waveformBufferR[m_waveformIndex] = static_cast<float>(f) * (255.f / 16.f);
    m_samplerR.write(f);
    m_waveformIndex = (m_waveformIndex + 1) % WaveformSize;

    m_tick++;

    static std::uint32_t buffered = 0;
    while (m_samplerL.pending())
    {
        m_buffer[m_bufferSize] = static_cast<std::uint16_t>(m_samplerL.read() * static_cast<float>(std::numeric_limits<std::uint16_t>::max()));
        m_buffer[m_bufferSize + 1] = static_cast<std::uint16_t>(m_samplerR.read() * static_cast<float>(std::numeric_limits<std::uint16_t>::max()));
        m_bufferSize = (m_bufferSize + 2) % BufferSize;

        if (m_bufferSize == 0 && audioDevice)
        {
            if (m_tick > TickMod)
            {
                buffered =  SDL_GetQueuedAudioSize(audioDevice);
                //std::cout << buffered << "\n";
                m_tick %= TickMod;
            }
            //limit the amount which is buffered and drop a frame of audio if needs be
            if (buffered > (BufferSize * 30))
            {
                buffered -= BufferSize;
                //how about rather than dropping a frame we introduce a frame skip which slows down the elss far behind we are?
                //skip every other frame in the extreme, else every 3 / 5 /9 etc
                //must keep track of how many were skipped so we can remove them from buffered count
            }
            else
            {
                SDL_QueueAudio(audioDevice, m_buffer.data(), static_cast<Uint32>(BufferSize) * sizeof(std::int16_t));
            }
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