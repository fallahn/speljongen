#pragma once

#include "Op.hpp"

#include <string>
#include <vector>

class OpCodeBuilder;
class OpCode final
{
public:
    OpCode();
    OpCode(const OpCodeBuilder&);
    std::size_t getOperandLength() const { return m_length; }
    const std::vector<Op>& getOps() const { return m_ops; }
    const std::string& getLabel() const { return m_label; }
    std::uint8_t getOpcode() const { return m_opcode; }

    operator bool() const { return m_isValid; }

private:
    std::uint8_t m_opcode;
    std::string m_label;
    std::vector<Op> m_ops;
    std::size_t m_length;
    bool m_isValid;
};