#include "OpCode.hpp"
#include "OpCodeBuilder.hpp"

OpCode::OpCode()
    : m_opcode  (0),
    m_length    (0),
    m_isValid   (false)
{

}

OpCode::OpCode(const OpCodeBuilder& builder)
    : m_opcode  (builder.getOpCode()),
    m_label     (builder.getLabel()),
    m_ops       (builder.getOps()),
    m_length    (0),
    m_isValid   (true)
{
    for (const auto& op : m_ops)
    {
        if (op.operandLength > m_length)
        {
            m_length = op.operandLength;
        }
    }
}