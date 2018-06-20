/*
MIT License

Copyright(c) 2017 Tomasz R?kawek(Coffee GB)
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

/*
Gameboy audio generator channel three, which
plays back 4 bit wave audio from 16 bytes memory
starting at 0xff30 (32 samples in total)
*/

#include "AudioChannel.hpp"

class ChannelThree final : public AudioChannel
{
public:
    explicit ChannelThree(std::vector<std::uint8_t>&);

    bool accepts(std::uint16_t) const override;
    void setByte(std::uint16_t, std::uint8_t) override;
    std::uint8_t getByte(std::uint16_t) const override;

    void start() override;
    void trigger() override;
    std::int32_t tick() override;

private:

    std::int32_t m_frequencyDivider;
    std::int32_t m_lastOutput;
    std::int32_t m_index;

    std::int32_t m_ticksSinceLastRead;
    std::uint16_t m_lastReadAddress;
    std::uint8_t m_buffer;
    bool m_triggered;

    std::uint8_t getVolume() const;
    std::uint8_t getWaveEntry();
    void resetFreqDivider();
};