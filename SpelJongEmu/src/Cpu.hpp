#pragma once

#include "OpCode.hpp"
#include "InterruptManager.hpp"
#include "Registers.hpp"

#include <vector>

class AddressSpace;
class SpeedMode;

class Cpu final
{
public:
    enum class State
    {
        OPCODE, EXT_OPCODE, OPERAND, RUNNING,
        IRQ_READ_IF, IRQ_READ_IE, IRQ_PUSH_1, IRQ_PUSH_2,
        IRQ_JUMP, STOPPED, HALTED
    };

    Cpu(AddressSpace&, InterruptManager&, SpeedMode&);

    void tick();
    
    Registers& getRegisters() { return m_registers; }
    void clearState();

    State getState() const { return m_state; }
    OpCode& getCurrentOpcode() { return m_currentOpcode; }
    const OpCode& getCurrentOpcode() const { return m_currentOpcode; }

private:

    Registers m_registers;
    AddressSpace& m_addressSpace;
    InterruptManager& m_interruptManager;
    SpeedMode& m_speedMode;

    std::uint8_t m_opcodeOne;
    std::uint8_t m_opcodeTwo;
    OpArgs m_opArgs;
    OpCode m_currentOpcode;

    std::vector<Op> m_ops;
    std::size_t m_opIndex;
    std::size_t m_operandIndex;

    State m_state;

    std::uint16_t m_opContext;
    std::uint8_t m_interruptFlag;
    std::uint8_t m_interruptEnabled;
    Interrupt::Type m_requestedInterrupt;

    std::uint32_t m_clockCycle;

    void handleInterrupt();
};