#pragma once

#include <cstdint>
#include <cstddef>

#include <array>
#include <functional>

class AddressSpace;
class Registers;
class InterruptManager;

using OpArgs = std::array<std::uint8_t, 2>;
using OpExecution = std::function<std::uint16_t(Registers&, AddressSpace&, const OpArgs&, std::uint16_t)>;

struct Op final
{
    bool readsMemory = false;
    bool writesMemory = false;
    std::size_t operandLength = 0;

    OpExecution execute =
        [](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t context)
    {
        return context;
    };

    std::function<void(InterruptManager& interruptManager)> switchInterrupts = [](InterruptManager&) {};
    std::function<bool(Registers& registers)>proceed = [](Registers&) { return true; };
    bool forceFinishCycle = false;
};