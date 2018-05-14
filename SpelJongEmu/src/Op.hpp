#pragma once

#include <cstdint>
#include <cstddef>

#include <array>

class AddressSpace;
class Registers;
class InterruptManager;

class Op
{
public:
    virtual ~Op() = default;

    virtual bool readsMemory() const { return false; }
    virtual bool writesMemory() const { return false; }
    virtual std::size_t operandLength() const { return 0; }

    //TODO find max op args
    virtual std::int32_t execute(Registers& registers, AddressSpace& addressSpace, const std::array<std::uint8_t, 2>& args, std::int32_t context)
    {
        return context;
    }

    virtual void switchInterrupts(InterruptManager& interruptManager) const {}
    virtual bool proceed(Registers& registers) const { return true; }
    virtual bool forceFinishCycle() const { return false; }
};