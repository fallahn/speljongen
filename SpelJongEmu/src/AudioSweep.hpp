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
//implements the frequency sweep register found on audio channel 1

#include "Consts.hpp"

#include <cstdint>

class AudioSweep final
{
public:
    AudioSweep();

    void start();
    void trigger();

    void setNr10(std::uint8_t);
    void setNr13(std::uint8_t);
    void setNr14(std::uint8_t);

    std::uint8_t getNr13() const;
    std::uint8_t getNr14() const;

    void tick();

    bool enabled() const;

private:

    std::int32_t m_period;
    bool m_negate;
    std::int32_t m_shift;

    std::int32_t m_timer;
    std::int32_t m_shadowFrequency;

    std::uint8_t m_nr13;
    std::uint8_t m_nr14;

    std::int32_t m_index;

    bool m_overflow;
    bool m_counterEnabled;
    bool m_negated;

    std::int32_t calculate();

    static constexpr std::uint32_t divisor = CYCLES_PER_SECOND / 128;
};