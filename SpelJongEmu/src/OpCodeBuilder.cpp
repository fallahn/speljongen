#include "OpCodeBuilder.hpp"

OpCodeBuilder::OpCodeBuilder(std::uint8_t opcode, const std::string& label)
    : m_opcode  (opcode),
    m_label     (label),
    m_lastType  (DataType::D8)
{

}

//public
OpCodeBuilder& OpCodeBuilder::copyByte(const std::string& target, const std::string& source)
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::load(const std::string& source)
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::loadWord(std::uint8_t val)
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::store(const std::string& target)
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::proceedIf(const std::string& condition)
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::push()
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::pop()
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::alu(const std::string& operation, const std::string& arg2)
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::alu(const std::string& operation, std::uint8_t)
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::alu(const std::string& operation)
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::aluHL(const std::string& operation)
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::bitHL(std::uint8_t)
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::clearZ()
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::switchInterrupts(bool enable, bool withDelay)
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::extraCycle()
{
    return *this;
}

OpCodeBuilder& OpCodeBuilder::forceFinish()
{
    return *this;
}