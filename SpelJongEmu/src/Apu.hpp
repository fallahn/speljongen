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

#include "AudioChannel.hpp"
#include "AudioOutput.hpp"

#include <array>
#include <memory>

class Apu final : public AddressSpace
{
public:
    explicit Apu(std::vector<std::uint8_t>&);

    std::string getLabel() const override { return "APU"; }

    bool accepts(std::uint16_t) const override;
    void setByte(std::uint16_t, std::uint8_t) override;
    std::uint8_t getByte(std::uint16_t) const override;

    void tick();
    void enableChannel(bool, std::int32_t);

    void enableColour(bool);

    const float* getWaveformL() const { return m_output.getWaveformL(); }
    const float* getWaveformR() const { return m_output.getWaveformR(); }
    std::size_t getWaveformSize() const { return m_output.getWaveformSize(); }

    bool playing() const { return m_output.running(); }

private:

    bool m_enabled;
    std::array<bool, 4> m_overrideEnabled = {};
    std::array<std::int32_t, 4u> m_channelOutputs = {};

    std::array<std::unique_ptr<AudioChannel>, 4u> m_channelGenerators;

    std::array<AddressSpace*, 0xff27 - 0xff10> m_addressMap;

    AudioOutput m_output;

    void start();
    void stop();
};