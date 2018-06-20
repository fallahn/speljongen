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
Implements the Gameboy's noise generator
*/

#include "AudioChannel.hpp"
#include "AudioEnvelope.hpp"

class PolyNomCounter final
{
public:

    bool tick()
    {
        if (--m_index == 0)
        {
            m_index = m_shiftedDivisor;
            return true;
        }
        return false;
    }

    void setNr43(std::uint8_t value)
    {
        auto clockShift = value >> 4;
        std::int32_t divisor = 0;
        switch (value & 0x7)
        {
        default: assert(false); return;
        case 0: divisor = 8; break;
        case 1: divisor = 16; break;
        case 2: divisor = 32; break;
        case 3: divisor = 48; break;
        case 4: divisor = 64; break;
        case 5: divisor = 80; break;
        case 6: divisor = 96; break;
        case 7: divisor = 112; break;
        }

        m_shiftedDivisor = divisor << clockShift;
        m_index = 1;
    }

private:
    std::int32_t m_shiftedDivisor = 0;
    std::int32_t m_index = 0;
};

class Lfsr final
{
public:
    void start() { reset(); }
    void reset() { m_lfsr = 0x7fff; }

    std::uint16_t nextBit(bool widthMode7)
    {
        bool x = ((m_lfsr & 1) ^ ((m_lfsr & 2) >> 1)) != 0;
        m_lfsr = m_lfsr >> 1;
        m_lfsr = m_lfsr | (x ? (1 << 14) : 0);
        if (widthMode7)
        {
            m_lfsr = m_lfsr | (x ? (1 << 6) : 0);
        }
        return 1 & ~m_lfsr;
    }

    std::uint16_t getValue() const
    {
        return m_lfsr;
    }

private:
    std::uint16_t m_lfsr = 0x7fff;
};

class ChannelFour final : public AudioChannel
{
public:
    explicit ChannelFour(std::vector<std::uint8_t>&);

    void setByte(std::uint16_t, std::uint8_t) override;
    std::uint8_t getByte(std::uint16_t) const override;

    void start() override;
    void trigger() override;
    std::int32_t tick() override;


private:
    VolumeEnvelope m_volumeEnvelope;
    PolyNomCounter m_counter;
    std::int32_t m_lastResult;
    Lfsr m_lfsr;

};