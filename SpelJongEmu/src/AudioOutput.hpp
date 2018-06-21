/*
MIT License

Copyright(c) 2018 Matt Marchant(Speljongen)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Sampler.hpp"

#include <cstdint>
#include <vector>

class AudioOutput final
{
public:
    AudioOutput();
    ~AudioOutput();
    AudioOutput(const AudioOutput&) = delete;
    AudioOutput(AudioOutput&&) = delete;
    AudioOutput& operator = (const AudioOutput&) = delete;
    AudioOutput& operator = (AudioOutput&&) = delete;

    void addSample(std::uint8_t, std::uint8_t);

    const float* getWaveformL() const { return m_waveformBufferL.data(); }
    const float* getWaveformR() const { return m_waveformBufferR.data(); }
    std::size_t getWaveformSize() const;

    void start();
    void stop();

    bool running() const { return m_running; }

private:

    std::int16_t m_left;
    std::int16_t m_right;

    std::vector<std::uint16_t> m_buffer;
    std::size_t m_bufferSize;

    std::vector<float> m_waveformBufferL;
    std::vector<float> m_waveformBufferR;
    std::size_t m_waveformIndex;

    std::uint32_t m_tick;

    Sampler m_samplerL;
    Sampler m_samplerR;

    std::int16_t to16Bit(std::uint8_t, std::vector<float>&);

    bool m_running;
};