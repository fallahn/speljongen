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

#include "AddressSpace.hpp"
#include "Consts.hpp"

class SampleCounter final
{
public:
    explicit SampleCounter(std::uint32_t);

    void start();
    void tick();
    void setLength(std::uint32_t);
    void setNr4(std::uint8_t);
    std::uint32_t getLength() const;
    bool isEnabled() const;
    void reset();

private:
    static constexpr std::uint32_t divisor = CYCLES_PER_SECOND / 256;
    std::uint32_t m_fullLength;
    std::uint32_t m_length;
    std::size_t m_index;
    bool m_enabled;
};

class AudioChannel : public AddressSpace
{
public:
    AudioChannel(std::vector<uint8_t>&, std::uint16_t offset, std::uint32_t length);
    
    virtual ~AudioChannel() = default;

    virtual std::int32_t tick() = 0;
    virtual void trigger() = 0;

    bool isEnabled() const;

    bool accepts(std::uint16_t) const override;

    virtual void setByte(std::uint16_t, std::uint8_t) override;

    virtual std::uint8_t getByte(std::uint16_t) const override;

    virtual void start() = 0;

    void stop();

    void setColour(bool c) { m_colour = c; }

protected:

    std::uint16_t getFrequency() const;
    bool updateCounter();
    std::uint16_t getOffset() const { return m_offset; }
    SampleCounter& getSampleCounter() { return m_sampleCounter; }
    void setDacEnabled(bool enabled) { m_dacEnabled = enabled; }
    bool getDacEnabled() const { return m_dacEnabled; }

    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool getEnabled() const { return m_enabled; }

    bool isColour() const { return m_colour; }

private:

    std::uint16_t m_offset;
    bool m_colour;
    bool m_enabled;
    bool m_dacEnabled;
    SampleCounter m_sampleCounter;
};