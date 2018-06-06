/*
MIT License

Copyright(c) 2017 Tomasz R?kawek(Coffee GB)
Copyright(c) 2018 Matt Marchant(Speljongen)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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