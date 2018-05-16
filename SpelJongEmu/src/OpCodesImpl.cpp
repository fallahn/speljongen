#include "OpCodesImpl.hpp"

std::vector<OpCode> OpCodes::Commands;
std::vector<OpCode> OpCodes::ExtCommands;

//public
void OpCodes::initOpCodes()
{
    std::vector<OpCodeBuilder> opcodes(0x100);
    std::vector<OpCodeBuilder> extOpcodes(0x100);
}

//private
OpCodeBuilder& OpCodes::regLoad(std::vector<std::unique_ptr<OpCodeBuilder>>& commands, std::uint8_t opcode, const std::string& target, const std::string& source)
{

}

OpCodeBuilder& OpCodes::regCmd(std::vector<std::unique_ptr<OpCodeBuilder>>& commands, std::uint8_t opcode, const std::string& label)
{
    commands[opcode] = std::make_unique<OpCodeBuilder>(opcode, label);
    return *commands[opcode];
}

OpCodeBuilder& OpCodes::regCmd(std::vector<std::unique_ptr<OpCodeBuilder>>& commands, std::pair<std::uint16_t, std::string> opcode, const std::string& label)
{

}