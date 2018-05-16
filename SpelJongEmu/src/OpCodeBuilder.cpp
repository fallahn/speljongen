#include "OpCodeBuilder.hpp"
#include "AluFunctions.hpp"
#include "InterruptManager.hpp"

#include <algorithm>

namespace
{
    Argument* findArg(const std::string& src)
    {
        auto result = std::find_if(arguments.begin(), arguments.end(),
            [&src](const std::unique_ptr<Argument>& a)
        {
            return a->getLabel() == src;
        });
        assert(result != arguments.end());
        return &*(*result);
    }

    OpExecution findAlu(const std::string& operation, DataType type)
    {
        if (operation == "INC")
        {
            if (type == DataType::D8)
            {
                return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
                {
                    return AluFunctions::inc(registers.getFlags(), static_cast<std::uint8_t>(value));
                };
            }
            else if (type == DataType::D16)
            {
                return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
                {
                    return AluFunctions::inc16(registers.getFlags(), value);
                };
            }
        }
        else if (operation == "DEC")
        {
            if (type == DataType::D8)
            {
                return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
                {
                    return AluFunctions::dec(registers.getFlags(), static_cast<std::uint8_t>(value));
                };
            }
            else if (type == DataType::D16)
            {
                return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
                {
                    return AluFunctions::dec16(registers.getFlags(), value);
                };
            }
        }
        else if (operation == "DAA")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::daa(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        else if (operation == "CPL")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::cpl(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        else if (operation == "SCF")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::scf(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        else if (operation == "CCF")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::ccf(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        else if (operation == "RLC")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::rlc(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        else if (operation == "RRC")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::rrc(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        else if (operation == "RL")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::rl(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        else if (operation == "RR")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::rr(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        else if (operation == "SLA")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::sla(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        else if (operation == "SRA")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::sra(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        else if (operation == "SWAP")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::swap(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        else if (operation == "SRL")
        {
            return [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t value)
            {
                return AluFunctions::srl(registers.getFlags(), static_cast<std::uint8_t>(value));
            };
        }
        return nullptr;
    }

    OpExecution findAlu2(const std::string& operation, DataType type, Argument* arg)
    {
        if (operation == "ADD")
        {
            if (type == DataType::D8 && arg->getDataType() == DataType::D8)
            {
                return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
                {
                    std::uint16_t val2 = arg->read(registers, addressSpace, args);
                    return AluFunctions::add(registers.getFlags(), static_cast<std::uint8_t>(val1), static_cast<std::uint8_t>(val2));
                };
            }
            else if (type == DataType::D16)
            {
                if (arg->getDataType() == DataType::R8)
                {
                    return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
                    {
                        std::uint16_t val2 = arg->read(registers, addressSpace, args);
                        return AluFunctions::add(registers.getFlags(), val1, static_cast<std::int8_t>(val2));
                    };
                }
                else if (arg->getDataType() == DataType::D16)
                {
                    return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
                    {
                        std::uint16_t val2 = arg->read(registers, addressSpace, args);
                        return AluFunctions::add(registers.getFlags(), val1, val2);
                    };
                }
            }
        }
        else if (operation == "ADD_SP")
        {
            return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
            {
                std::uint16_t val2 = arg->read(registers, addressSpace, args);
                return AluFunctions::add_sp(registers.getFlags(), val1, static_cast<std::int8_t>(val2));
            };
        }
        else if (operation == "ADC")
        {
            return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
            {
                std::uint16_t val2 = arg->read(registers, addressSpace, args);
                return AluFunctions::adc(registers.getFlags(), static_cast<std::uint8_t>(val1), static_cast<std::uint8_t>(val2));
            };
        }
        else if (operation == "SUB")
        {
            return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
            {
                std::uint16_t val2 = arg->read(registers, addressSpace, args);
                return AluFunctions::sub(registers.getFlags(), static_cast<std::uint8_t>(val1), static_cast<std::uint8_t>(val2));
            };
        }
        else if (operation == "SBC")
        {
            return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
            {
                std::uint16_t val2 = arg->read(registers, addressSpace, args);
                return AluFunctions::sbc(registers.getFlags(), static_cast<std::uint8_t>(val1), static_cast<std::uint8_t>(val2));
            };
        }
        else if (operation == "AND")
        {
            return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
            {
                std::uint16_t val2 = arg->read(registers, addressSpace, args);
                return AluFunctions::AND(registers.getFlags(), static_cast<std::uint8_t>(val1), static_cast<std::uint8_t>(val2));
            };
        }
        else if (operation == "OR")
        {
            return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
            {
                std::uint16_t val2 = arg->read(registers, addressSpace, args);
                return AluFunctions::OR(registers.getFlags(), static_cast<std::uint8_t>(val1), static_cast<std::uint8_t>(val2));
            };
        }
        else if (operation == "XOR")
        {
            return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
            {
                std::uint16_t val2 = arg->read(registers, addressSpace, args);
                return AluFunctions::XOR(registers.getFlags(), static_cast<std::uint8_t>(val1), static_cast<std::uint8_t>(val2));
            };
        }
        else if (operation == "CP")
        {
            return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
            {
                std::uint16_t val2 = arg->read(registers, addressSpace, args);
                return AluFunctions::cp(registers.getFlags(), static_cast<std::uint8_t>(val1), static_cast<std::uint8_t>(val2));
            };
        }
        else if (operation == "BIT")
        {
            return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
            {
                std::uint16_t val2 = arg->read(registers, addressSpace, args);
                return AluFunctions::bit(registers.getFlags(), static_cast<std::uint8_t>(val1), static_cast<std::uint8_t>(val2));
            };
        }
        else if (operation == "SET")
        {
            return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
            {
                std::uint16_t val2 = arg->read(registers, addressSpace, args);
                return AluFunctions::set(static_cast<std::uint8_t>(val1), static_cast<std::uint8_t>(val2));
            };
        }
        else if (operation == "RES")
        {
            return [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t val1)
            {
                std::uint16_t val2 = arg->read(registers, addressSpace, args);
                return AluFunctions::res(static_cast<std::uint8_t>(val1), static_cast<std::uint8_t>(val2));
            };
        }
    }

    OpExecution findAluD8(const std::string& operation, DataType type, std::uint8_t value)
    {
        if (operation == "ADD")
        {
            return [value](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t val1)
            {
                return AluFunctions::add(registers.getFlags(), static_cast<std::uint8_t>(val1), value);
            };
        }
        else if (operation == "ADC")
        {
            return [value](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t val1)
            {
                return AluFunctions::adc(registers.getFlags(), static_cast<std::uint8_t>(val1), value);
            };
        }
        else if (operation == "SUB")
        {
            return [value](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t val1)
            {
                return AluFunctions::sub(registers.getFlags(), static_cast<std::uint8_t>(val1), value);
            };
        }
        else if (operation == "SBC")
        {
            return [value](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t val1)
            {
                return AluFunctions::sbc(registers.getFlags(), static_cast<std::uint8_t>(val1), value);
            };
        }
        else if (operation == "AND")
        {
            return [value](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t val1)
            {
                return AluFunctions::AND(registers.getFlags(), static_cast<std::uint8_t>(val1), value);
            };
        }
        else if (operation == "OR")
        {
            return [value](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t val1)
            {
                return AluFunctions::OR(registers.getFlags(), static_cast<std::uint8_t>(val1), value);
            };
        }
        else if (operation == "XOR")
        {
            return [value](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t val1)
            {
                return AluFunctions::XOR(registers.getFlags(), static_cast<std::uint8_t>(val1), value);
            };
        }
        else if (operation == "CP")
        {
            return [value](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t val1)
            {
                return AluFunctions::cp(registers.getFlags(), static_cast<std::uint8_t>(val1), value);
            };
        }
        else if (operation == "BIT")
        {
            return [value](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t val1)
            {
                return AluFunctions::bit(registers.getFlags(), static_cast<std::uint8_t>(val1), value);
            };
        }
        else if (operation == "SET")
        {
            return [value](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t val1)
            {
                return AluFunctions::set(static_cast<std::uint8_t>(val1), value);
            };
        }
        else if (operation == "RES")
        {
            return [value](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t val1)
            {
                return AluFunctions::res(static_cast<std::uint8_t>(val1), value);
            };
        }
    }
}

OpCodeBuilder::OpCodeBuilder(std::uint8_t opcode, const std::string& label)
    : m_opcode  (opcode),
    m_label     (label),
    m_lastType  (DataType::D8)
{

}

//public
OpCodeBuilder& OpCodeBuilder::copyByte(const std::string& target, const std::string& source)
{
    load(source);
    store(target);
    return *this;
}

OpCodeBuilder& OpCodeBuilder::load(const std::string& source)
{
    auto* arg = findArg(source);
    m_lastType = arg->getDataType();

    m_ops.emplace_back();

    auto& op = m_ops.back();
    op.readsMemory = arg->isMemory();
    op.operandLength = arg->getOperandLength();
    op.execute = [arg](Registers& registers, AddressSpace& addressSpace, const std::array<std::uint8_t, 2u>& args, std::uint16_t)
    {
        return arg->read(registers, addressSpace, args);
    };

    return *this;
}

OpCodeBuilder& OpCodeBuilder::loadWord(std::uint16_t val)
{
    m_lastType = DataType::D16;

    m_ops.emplace_back();
    m_ops.back().execute = 
        [val](Registers&, AddressSpace&, const std::array<std::uint8_t, 2u>&, std::uint16_t)
    {
        return val;
    };

    return *this;
}

OpCodeBuilder& OpCodeBuilder::store(const std::string& target)
{
    auto* arg = findArg(target);
    if (m_lastType == DataType::D16 && arg->id() == Argument::_A16)
    {
        m_ops.emplace_back();
        auto& op = m_ops.back();
        op.writesMemory = arg->isMemory();
        op.operandLength = arg->getOperandLength();
        op.execute = [](Registers&, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t context)
        {
            addressSpace.setByte(BitUtil::toWord(args), static_cast<std::uint8_t>(context));
            return context;
        };

        m_ops.emplace_back();
        auto& op2 = m_ops.back();
        op2.writesMemory = arg->isMemory();
        op2.operandLength = arg->getOperandLength();
        op2.execute = [](Registers&, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t context)
        {
            addressSpace.setByte(BitUtil::toWord(args) + 1, static_cast<std::uint8_t>((context & 0xff00) >> 8));
            return context;
        };
    }
    else if (m_lastType == arg->getDataType())
    {
        m_ops.emplace_back();
        auto& op = m_ops.back();
        op.writesMemory = arg->isMemory();
        op.operandLength = arg->getOperandLength();
        op.execute = [arg](Registers& registers, AddressSpace& addressSpace, const OpArgs& args, std::uint16_t context)
        {
            arg->write(registers, addressSpace, args, context);
            return context;
        };
    }
    return *this;
}

OpCodeBuilder& OpCodeBuilder::proceedIf(const std::string& condition)
{
    std::int32_t conditionVal = -1;
    if (condition == "NZ") conditionVal = 0;
    else if (condition == "Z") conditionVal = 1;
    else if (condition == "NC") conditionVal = 2;
    else if (condition == "C") conditionVal = 3;

    m_ops.emplace_back();
    m_ops.back().proceed = [conditionVal](Registers& registers)
    {
        switch (conditionVal)
        {
        default: return false;
        case 0:
            return !registers.getFlags().isSet(Flags::Z);
        case 1:
            return registers.getFlags().isSet(Flags::Z);
        case 2:
            return !registers.getFlags().isSet(Flags::C);
        case 3:
            return registers.getFlags().isSet(Flags::C);
        }
        return false;
    };

    return *this;
}

OpCodeBuilder& OpCodeBuilder::push()
{
    m_ops.emplace_back();
    auto& op = m_ops.back();
    op.writesMemory = true;
    op.execute = [](Registers& registers, AddressSpace& addressSpace, const OpArgs&, std::uint16_t context)
    {
        registers.setSP(AluFunctions::dec16(registers.getFlags(), registers.getSP()));
        addressSpace.setByte(registers.getSP(), static_cast<std::uint8_t>((context & 0xff00) >> 8));
        return context;
    };

    m_ops.emplace_back();
    auto& op2 = m_ops.back();
    op2.writesMemory = true;
    op2.execute = [](Registers& registers, AddressSpace& addressSpace, const OpArgs&, std::uint16_t context)
    {
        registers.setSP(AluFunctions::dec16(registers.getFlags(), registers.getSP()));
        addressSpace.setByte(registers.getSP(), static_cast<std::uint8_t>(context & 0x00ff));
        return context;
    };

    return *this;
}

OpCodeBuilder& OpCodeBuilder::pop()
{
    m_lastType = DataType::D16;
    m_ops.emplace_back();
    auto& op = m_ops.back();
    op.readsMemory = true;
    op.execute = [](Registers& registers, AddressSpace& addressSpace, const OpArgs&, std::uint16_t context)
    {
        std::uint8_t lsb = addressSpace.getByte(registers.getSP());
        registers.setSP(AluFunctions::inc16(registers.getFlags(), registers.getSP()));
        return lsb;
    };

    m_ops.emplace_back();
    auto& op2 = m_ops.back();
    op2.readsMemory = true;
    op2.execute = [](Registers& registers, AddressSpace& addressSpace, const OpArgs&, std::uint16_t context)
    {
        std::uint8_t msb = addressSpace.getByte(registers.getSP());
        registers.setSP(AluFunctions::inc16(registers.getFlags(), registers.getSP()));
        return context | (msb << 8);
    };

    return *this;
}

OpCodeBuilder& OpCodeBuilder::alu(const std::string& operation, const std::string& arg2)
{
    auto* arg = findArg(arg2);
    m_ops.emplace_back();
    auto& op = m_ops.back();
    op.readsMemory = arg->isMemory();
    op.operandLength = arg->getOperandLength();
    op.execute = findAlu2(operation, m_lastType, arg);

    if (m_lastType == DataType::D16)
    {
        extraCycle();
    }

    return *this;
}

OpCodeBuilder& OpCodeBuilder::alu(const std::string& operation, std::uint8_t value)
{
    m_ops.emplace_back();
    auto& op = m_ops.back();
    op.execute = findAluD8(operation, m_lastType, value);

    if (m_lastType == DataType::D16)
    {
        extraCycle();
    }

    return *this;
}

OpCodeBuilder& OpCodeBuilder::alu(const std::string& operation)
{
    m_ops.emplace_back();
    auto& op = m_ops.back();
    op.execute = findAlu(operation, m_lastType);

    if (m_lastType == DataType::D16)
    {
        extraCycle();
    }
    return *this;
}

OpCodeBuilder& OpCodeBuilder::aluHL(const std::string& operation)
{
    load("HL");

    m_ops.emplace_back();
    auto& op = m_ops.back();
    op.execute = findAlu(operation, DataType::D16);

    store("HL");
    return *this;
}

OpCodeBuilder& OpCodeBuilder::bitHL(std::uint8_t bit)
{
    m_ops.emplace_back();
    auto& op = m_ops.back();
    op.readsMemory = true;
    op.execute = [bit](Registers& registers, AddressSpace& addressSpace, const OpArgs&, std::uint16_t context)
    {
        std::uint8_t value = addressSpace.getByte(registers.getHL());
        auto& flags = registers.getFlags();
        flags.set(Flags::N, false);
        flags.set(Flags::H, true);

        if (bit < 8)
        {
            flags.set(Flags::Z, !BitUtil::getBit(value, bit));
        }

        return context;
    };

    return *this;
}

OpCodeBuilder& OpCodeBuilder::clearZ()
{
    m_ops.emplace_back();
    auto& op = m_ops.back();
    op.execute = [](Registers& registers, AddressSpace&, const OpArgs&, std::uint16_t context)
    {
        registers.getFlags().set(Flags::Z, false);
        return context;
    };

    return *this;
}

OpCodeBuilder& OpCodeBuilder::switchInterrupts(bool enable, bool withDelay)
{
    m_ops.emplace_back();
    auto& op = m_ops.back();
    if (enable)
    {
        op.switchInterrupts = [withDelay](InterruptManager& interruptManager)
        {
            interruptManager.enableInterrupts(withDelay);
        };
    }
    else
    {
        op.switchInterrupts = [withDelay](InterruptManager& interruptManager)
        {
            interruptManager.disableInterrupts(withDelay);
        };
    }

    return *this;
}

OpCodeBuilder& OpCodeBuilder::extraCycle()
{
    m_ops.emplace_back();
    m_ops.back().readsMemory = true;
    return *this;
}

OpCodeBuilder& OpCodeBuilder::forceFinish()
{
    m_ops.emplace_back();
    m_ops.back().forceFinishCycle = true;
    return *this;
}