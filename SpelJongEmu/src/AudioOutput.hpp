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

#include <SFML/Audio/SoundStream.hpp>

#include <cstdint>
#include <vector>

class AudioOutput final : public sf::SoundStream
{
public:
    AudioOutput();

    void addSample(std::uint8_t, std::uint8_t);

    const float* getWaveformL() const { return m_waveformBufferL.data(); }
    const float* getWaveformR() const { return m_waveformBufferR.data(); }
    std::size_t getWaveformSize() const { return m_bufferSize / 2; }

private:

    sf::Mutex m_mutex;

    std::vector<sf::Int16> m_buffer;
    std::vector<sf::Int16> m_outBuffer;
    std::vector<sf::Int16> m_emptyBuffer;
    std::size_t m_bufferSize;
    std::uint32_t m_tick;

    std::vector<float> m_waveformBufferL;
    std::vector<float> m_waveformBufferR;

    sf::Int16 to16Bit(std::uint8_t, std::vector<float>&);

    bool onGetData(Chunk&) override;
    void onSeek(sf::Time)override {}

};