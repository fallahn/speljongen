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

#include "OpCode.hpp"
#include "InterruptManager.hpp"
#include "CpuRegisters.hpp"

#include <vector>
#include <fstream>

class AddressSpace;
class SpeedMode;
class Display;

class Cpu final
{
public:
    enum State
    {
        OPCODE = 1,
        EXT_OPCODE = 2,
        OPERAND = 4,
        RUNNING = 8,
        IRQ_READ_IF = 16,
        IRQ_READ_IE = 32,
        IRQ_PUSH_1 = 64,
        IRQ_PUSH_2 = 128,
        IRQ_JUMP = 256, 
        STOPPED = 512,
        HALTED = 1024,
        OPCODE_HALTED_STOPPED = OPCODE | HALTED | STOPPED,
        HALTED_STOPPED = HALTED | STOPPED,
        HAS_INTERRUPT_REQUEST = IRQ_READ_IF | IRQ_READ_IE | IRQ_PUSH_1 | IRQ_PUSH_2 | IRQ_JUMP
    };

    Cpu(AddressSpace&, InterruptManager&, SpeedMode&, Display&);

    bool tick(); //returns false when in the middle of a an op
    
    Registers& getRegisters() { return m_registers; }
    const Registers& getRegisters() const { return m_registers; }
    void clearState();

    State getState() const { return m_state; }
    OpCode& getCurrentOpcode() { return m_currentOpcode; }
    const OpCode& getCurrentOpcode() const { return m_currentOpcode; }

private:

    Registers m_registers;
    AddressSpace& m_addressSpace;
    InterruptManager& m_interruptManager;
    SpeedMode& m_speedMode;
    Display& m_display;

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
    bool m_haltBugMode;

    std::ofstream m_logFile;

    void handleInterrupt();
};