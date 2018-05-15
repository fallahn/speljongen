#include "OpCode.hpp"
#include "OpCodeBuilder.hpp"

OpCode::OpCode(const OpCodeBuilder& builder)
    : m_opcode  (builder.getOpCode()),
    m_label     (builder.getLabel()),
    m_ops       (builder.getOps()),
    m_length    (m_ops.size()) //??? check list.stream().mapToInt();
{

}