#pragma once

#include "RingBuffer.hpp"

#include <array>

class Sampler final
{
public:
    Sampler(double, double, std::size_t = 0);
    double read() { return m_buffer.read(); }
    void write(double);
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