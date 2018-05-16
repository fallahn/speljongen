#include "OpCode.hpp"
#include "OpCodeBuilder.hpp"

OpCode::OpCode(const OpCodeBuilder& builder)
    : m_opcode  (builder.getOpCode()),
    m_label     (builder.getLabel()),
    m_ops       (builder.getOps()),
    m_length    (0)
{
    for (const auto& op : m_ops)
    {
        if (op.operandLength > m_length)
        {
            m_length = op.operandLength;
        }
    }
}