#pragma once

//assembles opcodes from arguments that create ops

#include "Op.hpp"
#include "Argument.hpp"
#include "OpCode.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>

static std::array<std::unique_ptr<Argument>, Argument::ID::Count> arguments =
{
    std::make_unique<ArgumentA>(),
    std::make_unique<ArgumentB>(),
    std::make_unique<ArgumentC>(),
    std::make_unique<ArgumentD>(),
    std::make_unique<ArgumentE>(),
    std::make_unique<ArgumentH>(),
    std::make_unique<ArgumentL>(),
    std::make_unique<ArgumentAF>(),
    std::make_unique<ArgumentBC>(),
    std::make_unique<ArgumentDE>(),
    std::make_unique<ArgumentHL>(),
    std::make_unique<ArgumentSP>(),
    std::make_unique<ArgumentPC>(),
    std::make_unique<ArgumentD8>(),
    std::make_unique<ArgumentD16>(),
    std::make_unique<ArgumentR8>(),
    std::make_unique<ArgumentA16>(),
    std::make_unique<Argument_BC>(),
    std::make_unique<Argument_DE>(),
    std::make_unique<Argument_HL>(),
    std::make_unique<Argument_A8>(),
    std::make_unique<Argument_A16>(),
    std::make_unique<Argument_C>()
};

class OpCodeBuilder final
{
public:
    OpCodeBuilder(std::uint8_t opcode, const std::string& label);

    OpCodeBuilder& copyByte(const std::string& target, const std::string& source);
    OpCodeBuilder& load(const std::string& source);
    OpCodeBuilder& loadWord(std::uint16_t);
    OpCodeBuilder& store(const std::string& target);
    OpCodeBuilder& proceedIf(const std::string& condition);
    OpCodeBuilder& push();
    OpCodeBuilder& pop();
    OpCodeBuilder& alu(const std::string& operation, const std::string& arg2);
    OpCodeBuilder& alu(const std::string& operation, std::uint8_t);
    OpCodeBuilder& alu(const std::string& operation);
    OpCodeBuilder& aluHL(const std::string& operation);
    OpCodeBuilder& bitHL(std::uint8_t);
    OpCodeBuilder& clearZ();
    OpCodeBuilder& switchInterrupts(bool enable, bool withDelay);
    OpCodeBuilder& extraCycle();
    OpCodeBuilder& forceFinish();

    std::uint8_t getOpCode() const { return m_opcode; }
    const std::string& getLabel() const { return m_label; }
    const std::vector<Op>& getOps() const { return m_ops; }
    
private:
    std::uint8_t m_opcode;
    std::string m_label;
    std::vector<Op> m_ops;
    DataType m_lastType;
};
