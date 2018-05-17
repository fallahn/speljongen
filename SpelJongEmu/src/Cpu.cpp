#include "Cpu.hpp"
#include "AddressSpace.hpp"
#include "SpeedMode.hpp"
#include "OpCodesImpl.hpp"

Cpu::Cpu(AddressSpace& addressSpace, InterruptManager& interruptManager, SpeedMode& speedMode)
    : m_addressSpace        (addressSpace),
    m_interruptManager      (interruptManager),
    m_speedMode             (speedMode),
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
    m_clockCycle            (0)
{
    //TODO compile opcodes here
}

//public
void Cpu::tick()
{
    m_clockCycle = (m_clockCycle + 1) % (4 / m_speedMode.getSpeedMode());
    if (m_clockCycle != 0) return;

    //TODO can we improve this a bit using bit flags on the state?
    if (m_state == State::OPCODE ||
        m_state == State::HALTED ||
        m_state == State::STOPPED)
    {
        if (m_interruptManager.isIME() && m_interruptManager.isInterruptRequested())
        {
            //TODO enable LCD if STOPPED
            m_state = State::IRQ_READ_IF;
        }
    }

    if (m_state == State::IRQ_READ_IF ||
        m_state == State::IRQ_READ_IE ||
        m_state == State::IRQ_PUSH_1 ||
        m_state == State::IRQ_PUSH_2 ||
        m_state == State::IRQ_JUMP)
    {
        handleInterrupt();
        return;
    }

    if (m_state == State::HALTED && m_interruptManager.isInterruptRequested())
    {
        m_state = State::OPCODE;
    }

    if (m_state == State::HALTED || m_state == State::STOPPED)
    {
        return;
    }


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
            return;
        case State::OPCODE:
            clearState();
            m_opcodeOne = m_addressSpace.getByte(pc);
            accessedMemory = true;
            if (m_opcodeOne == 0xcb) //TODO look these up out of interest
            {
                m_state = State::EXT_OPCODE;
            }
            else if (m_opcodeOne == 0x10)
            {
                assert(!OpCodes::Commands.empty()); //Haven't called init yet!!
                m_state = State::EXT_OPCODE;
            }
            else
            {
                assert(!OpCodes::Commands.empty()); //Haven't called init yet!!
                m_state = State::OPERAND;
            }
            //TODO pause here if debug mode enabled
            m_registers.incrementPC();
            break;
        case State::EXT_OPCODE:

            break;
        case State::OPERAND:

            break;
        case State::RUNNING:

            break;
        }
    }
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
        m_addressSpace.setByte(m_registers.getSP(), static_cast<std::uint8_t>(m_registers.getSP() & 0x00ff));
        m_state = State::IRQ_JUMP;

        break;
    case State::IRQ_JUMP:
        m_registers.setPC(Interrupts[m_requestedInterrupt]);
        m_requestedInterrupt = Interrupt::Count;
        m_state = State::OPCODE;
        break;
    }
}