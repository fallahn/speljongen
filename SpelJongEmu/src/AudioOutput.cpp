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
    const std::uint32_t SampleRate = 44100;
    const std::size_t BufferSize = 1024;
    const std::array<std::int16_t, 2000> buns = {};
    

    const std::uint32_t TickCount = (CYCLES_PER_SECOND / SampleRate) + 2;
    const std::size_t WaveformSize = (BufferSize * TickCount) / 2;

    SDL_AudioDeviceID audioDevice = 0;
}

AudioOutput::AudioOutput()
    : m_left        (0),
    m_right         (0),
    m_bufferSize    (0),
    m_waveformIndex (0),
    m_tick          (0),
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
    /*else
    {
        SDL_PauseAudioDevice(audioDevice, 0);
    }*/

    float buns = 1.f / (static_cast<float>(CYCLES_PER_SECOND) / SampleRate);
    std::cout << "can we take a sample this many seconds apart? " << buns << "\n";
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
    m_left = to16Bit(left, m_waveformBufferL);
    m_right = to16Bit(right, m_waveformBufferR);
    m_waveformIndex = (m_waveformIndex + 1) % WaveformSize;


    //TODO - can we better represent this as a float
    //for more precision? And move back to own thread..? ugh
    //or can we re-sync by flushing the queue?
    if (m_tick++ == 0)
    {
        m_buffer[m_bufferSize] = m_left;
        m_buffer[m_bufferSize + 1] = m_right;
        m_bufferSize = (m_bufferSize + 2) % BufferSize;

        if (m_bufferSize == 0 && audioDevice)
        {
            SDL_QueueAudio(audioDevice, m_buffer.data(), static_cast<Uint32>(BufferSize) * sizeof(std::int16_t));
            m_bufferSize = 0;
        }
    }
    else
    {
        m_tick %= TickCount;
    }
}

std::size_t AudioOutput::getWaveformSize() const { return WaveformSize; }

void AudioOutput::start()
{
    if (audioDevice)
    {
        SDL_PauseAudioDevice(audioDevice, 0);
        SDL_ClearQueuedAudio(audioDevice);
        m_running = true;
    }
    m_tick = 0;
    m_bufferSize = 0;
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
std::int16_t AudioOutput::to16Bit(std::uint8_t value, std::vector<float>& waveformBuffer)
{
    float f = std::min(1.f, static_cast<float>(value) / 255.f);
    /*f *= 2.f;
    f -= 1.f;*/
    waveformBuffer[m_waveformIndex] = f * (255.f / 16.f);

    f *= static_cast<float>(/*std::numeric_limits<std::int16_t>::max()*/32000);

    return static_cast<std::int16_t>(f);
}