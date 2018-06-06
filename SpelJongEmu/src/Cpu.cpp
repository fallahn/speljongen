#include "Cpu.hpp"
#include "Display.hpp"
#include "AddressSpace.hpp"
#include "SpeedMode.hpp"
#include "OpCodesImpl.hpp"

#include <iostream>
#include <iomanip>

namespace
{
    Registers lastState;
}

Cpu::Cpu(AddressSpace& addressSpace, InterruptManager& interruptManager, SpeedMode& speedMode, Display& display)
    : m_addressSpace        (addressSpace),
    m_interruptManager      (interruptManager),
    m_speedMode             (speedMode),
    m_display               (display),
    m_opcodeOne             (0),
    m_opcodeTwo             (0),
    m_opArgs                ({}),
    m_opIndex               (0),
    m_operandIndex          (0),
    m_state                 (State::OPCODE),
    m_opContext             (0),
    m_interruptFlag         (0),
    m_interruptEnabled      (0),
    m_requestedInterrupt    (Interrupt::Type::Count),
    m_clockCycle            (0),
    m_haltBugMode           (false)
{
    //compile opcodes
    OpCodes::init();

    m_logFile.open("pc.log", std::ios::out);
}

//public
bool Cpu::tick()
{
    m_clockCycle = (m_clockCycle + 1) % (4 / m_speedMode.getSpeedMode());
    if (m_clockCycle != 0) return false;


    if(m_state & OPCODE_HALTED_STOPPED)
    {
        if (m_interruptManager.isIME() && m_interruptManager.isInterruptRequested())
        {
            if (m_state == State::STOPPED)
            {
                m_display.clear(true);
            }
            m_state = State::IRQ_READ_IF;
        }
    }

    if(m_state & HAS_INTERRUPT_REQUEST)
    {
        //std::cout << "Interrupt!\n";
        handleInterrupt();
        return true;
    }

    if (m_state == State::HALTED && m_interruptManager.isInterruptRequested())
    {
        m_state = State::OPCODE;
    }

    if(m_state & HALTED_STOPPED)
    {
        return true;
    }

    //static int thingCounter = 0;
    //execute execute
    bool accessedMemory = false;
    while (1)
    {
        std::uint16_t pc = m_registers.getPC();
        switch (m_state)
        {
        default: 
        //case State::HALTED:
        //case State::STOPPED:
            return true;
        case State::OPCODE: //check current opcode, jump to extend opcodes or switch to collecting operands
            clearState();
            m_opcodeOne = m_addressSpace.getByte(pc);
            //thingCounter++;
            //m_logFile << std::hex << /*std::setfill('0') << std::setw(2) <<*/ pc << " " << (int)m_addressSpace.getByte(m_registers.getSP()) << "\n";
            
            /*if (pc == 0x52f7)
            {
                int buns = 0;
            }
            lastState = m_registers;*/

            accessedMemory = true;
            if (m_opcodeOne == 0xcb) //jumps to extended ops
            {
                m_state = State::EXT_OPCODE;
            }
            else if (m_opcodeOne == 0x10)
            {
                assert(!OpCodes::Commands.empty()); //Haven't called init yet!!
                m_currentOpcode = OpCodes::Commands[m_opcodeOne];
                m_state = State::EXT_OPCODE;
            }
            else
            {
                assert(!OpCodes::Commands.empty()); //Haven't called init yet!!
                m_currentOpcode = OpCodes::Commands[m_opcodeOne];
                assert(m_currentOpcode); //invalid opcode!
                m_state = State::OPERAND;
            }
            //pause here if debug mode enabled
            if (!m_haltBugMode)
            {
                m_registers.incrementPC();
            }
            else
            {
                m_haltBugMode = false;
            }
            break;
        case State::EXT_OPCODE: //check valid extended opcode and start collecting operands
            if (accessedMemory) return true;
            accessedMemory = true;
            m_opcodeTwo = m_addressSpace.getByte(pc);
            if (!m_currentOpcode)
            {
                m_currentOpcode = OpCodes::ExtCommands[m_opcodeTwo];
                assert(m_currentOpcode); //uh oh.
            }
            m_state = State::OPERAND;
            m_registers.incrementPC();
            break;
        case State::OPERAND: //collect any operands for current opcode
            while (m_operandIndex < m_currentOpcode.getOperandLength())
            {
                if (accessedMemory) return true;

                accessedMemory = true;
                m_opArgs[m_operandIndex++] = m_addressSpace.getByte(pc);
                m_registers.incrementPC();
            }
            m_ops = m_currentOpcode.getOps();
            m_state = State::RUNNING;
            break;
        case State::RUNNING: //execute opcode with any operands as parameters
            if (m_opcodeOne == 0x10)
            {
                if (m_speedMode.onStop())
                {
                    m_state = State::OPCODE;
                }
                else
                {
                    m_state = State::STOPPED;
                    m_display.clear();
                }
                return true;
            }
            else if (m_opcodeOne == 0x76) //HALT
            {
                if (m_interruptManager.isHaltBug())
                {
                    m_state = State::OPCODE;
                    m_haltBugMode = true;
                }
                else
                {
                    m_state = State::HALTED;
                }
                return true;
            }

            if (m_opIndex < m_ops.size())
            {
                const auto& op = m_ops[m_opIndex];
                bool opAccessesMemory = op.readsMemory || op.writesMemory;
                if (accessedMemory && opAccessesMemory)
                {
                    return true;
                }
                m_opIndex++;

                //TODO sprite debug output

                m_opContext = op.execute(m_registers, m_addressSpace, m_opArgs, m_opContext);
                op.switchInterrupts(m_interruptManager);

                if (!op.proceed(m_registers))
                {
                    m_opIndex = m_ops.size();
                    break;
                }

                if (op.forceFinishCycle)
                {
                    return true;
                }

                if (opAccessesMemory)
                {
                    accessedMemory = true;
                }
            }

            if (m_opIndex >= m_ops.size())
            {
                m_state = State::OPCODE;
                m_operandIndex = 0;
                m_interruptManager.onInstructionFinished();
                return true;
            }

            break;
        }
    }

    return true;
}

void Cpu::clearState()
{
    m_opcodeOne = 0;
    m_opcodeTwo = 0;
    m_currentOpcode = {};
    m_ops.clear();

    m_opArgs = {};
    m_operandIndex = 0;

    m_opIndex = 0;
    m_opContext = 0;

    m_interruptFlag = 0;
    m_interruptEnabled = 0;
    m_requestedInterrupt = Interrupt::Count;
}

//private
void Cpu::handleInterrupt()
{
    switch (m_state)
    {
    default: break;
    case State::IRQ_READ_IF:
        m_interruptFlag = m_addressSpace.getByte(0xff0f);
        m_state = State::IRQ_READ_IE;
        break;
    case State::IRQ_READ_IE:
        m_interruptEnabled = m_addressSpace.getByte(0xffff);
        m_requestedInterrupt = Interrupt::Type::Count;

        for (auto i = 0; i < Interrupt::Count; ++i)
        {
            if ((m_interruptFlag & m_interruptEnabled & (1 << i)) != 0)
            {
                m_requestedInterrupt = static_cast<Interrupt::Type>(i);
                break;
            }
        }
        m_interruptManager.flush();

        if (m_requestedInterrupt == Interrupt::Count)
        {
            m_state = State::OPCODE;
        }
        else
        {
            m_state = State::IRQ_PUSH_1;
            m_interruptManager.disableInterrupts(false);
        }

        break;
    case State::IRQ_PUSH_1:
        m_registers.decrementSP();
        m_addressSpace.setByte(m_registers.getSP(), (m_registers.getPC() & 0xff00) >> 8);
        m_state = State::IRQ_PUSH_2;
        break;

    case State::IRQ_PUSH_2:
        m_registers.decrementSP();
        m_addressSpace.setByte(m_registers.getSP(), static_cast<std::uint8_t>(m_registers.getPC() & 0x00ff));
        m_state = State::IRQ_JUMP;

        break;
    case State::IRQ_JUMP:
        m_registers.setPC(Interrupts[m_requestedInterrupt]);
        m_requestedInterrupt = Interrupt::Count;
        m_state = State::OPCODE;
        break;
    }
}