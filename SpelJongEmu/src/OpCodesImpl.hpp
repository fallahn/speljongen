#pragma once

/*
Used in conjustion with the opcode builder class to compile opcodes
at runtime from micro-ops
*/

#include "OpCode.hpp"
#include "OpCodeBuilder.hpp"

#include <vector> //TODO can we replace with static array?

class OpCodes final
{
public:
    static std::vector<OpCode> Commands;
    static std::vector<OpCode> ExtCommands;

    static void init();

private:
    static OpCodeBuilder& regLoad(std::vector<std::unique_ptr<OpCodeBuilder>>&, std::uint8_t, const std::string&, const std::string&);
    static OpCodeBuilder& regCmd(std::vector<std::unique_ptr<OpCodeBuilder>>&, std::uint8_t, const std::string&);
    static OpCodeBuilder& regCmd(std::vector<std::unique_ptr<OpCodeBuilder>>&, std::pair<std::uint8_t, std::string>, const std::string&);
};