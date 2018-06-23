#pragma once

/*
Interpolated rampling of audio output.
See https://stackoverflow.com/q/1125666/6740859
*/

#include "RingBuffer.hpp"

#include <array>

class Sampler final
{
public:
    Sampler(double, double, std::size_t = 0);
    double pop() { return m_buffer.pop(); }
    void push(double); //values should be normalised.
    bool pending() const { return m_buffer.pending(); }
    void reset(double, double, std::size_t);

private:
    double m_inputFreq = 0.0;
    double m_outputFreq = 0.0;
    double m_ratio = 0.0;
    double m_fraction = 0.0;
    std::array<double, 4> m_history = {};
    RingBuffer<double> m_buffer;
};