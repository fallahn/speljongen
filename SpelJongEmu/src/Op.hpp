#pragma once

#include <cstdint>
#include <cstddef>

#include <array>
#include <functional>

class AddressSpace;
class Registers;
class InterruptManager;

struct Op final
{
    bool readsMemory = false;
    bool writesMemory = false;
    std::size_t operandLength = 0;

    std::function<std::uint8_t(Registers&, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint8_t)> execute =
        [](Registers& registers, AddressSpace& addressSpace, const std::array<std::uint8_t, 2>& args, std::uint8_t context)
    {
        return context;
    };

    std::function<void(InterruptManager& interruptManager)> switchInterrupts;
    std::function<bool(Registers& registers)>proceed;
    bool forceFinishCycle = false;
};