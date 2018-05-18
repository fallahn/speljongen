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

    explicit InterruptManager(bool isColour);

    AddressSpace::Type type() const override { return AddressSpace::Interrupt; }

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