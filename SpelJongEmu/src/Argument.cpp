#include "Argument.hpp"

Argument::Argument()
    : m_operandLength   (0),
    m_memory            (false),
    m_dataType          (DataType::D8)
{

}

Argument::Argument(const std::string& label, std::size_t opLen, bool memory, DataType dt)
    : m_label       (label),
    m_operandLength (opLen),
    m_memory        (memory),
    m_dataType      (dt)
{

}

//public
std::size_t Argument::getOperandLength() const
{
    return m_operandLength;
}

bool Argument::isMemory() const
{
    return m_memory;
}

DataType Argument::getDataType() const
{
    return m_dataType;
}

const std::string& Argument::getLabel() const
{
    return m_label;
}