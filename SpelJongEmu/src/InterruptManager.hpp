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

#include <array>

namespace Interrupt
{
    enum Type
    {
        VBlank,
        LCDC,
        Timer,
        Serial,
        P10_13,
        Count
    };
};

static const std::array<std::uint16_t, Interrupt::Count> Interrupts = 
{
    (0x40),
    (0x48),
    (0x50),
    (0x58),
    (0x60)
};

class InterruptManager final : public AddressSpace
{
public:

    explicit InterruptManager(std::vector<std::uint8_t>&);

    void enableColour(bool colour) { m_isColour = colour; }

    std::string getLabel() const override { return "Interrupt Manager"; }

    void enableInterrupts(bool useDelay);

    void disableInterrupts(bool useDelay);

    void requestInterrupt(Interrupt::Type);

    void onInstructionFinished();

    bool isIME() const;

    bool isInterruptRequested() const;

    void flush();

    bool isHaltBug() const;

    bool accepts(std::uint16_t addres) const override;

    void setByte(std::uint16_t address, std::uint8_t value) override;

    std::uint8_t getByte(std::uint16_t address) const override;

private:

    bool m_isColour;
    bool m_ime;
    std::uint8_t m_interruptFlag;
    std::uint8_t m_interruptEnabled;
    std::int8_t m_pendingEnableInterrupts;
    std::int8_t m_pendingDisableInterrupts;
};