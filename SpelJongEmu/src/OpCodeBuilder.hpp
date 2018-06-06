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

    void copyByte(const std::string& target, const std::string& source);
    void load(const std::string& source);
    void loadWord(std::uint16_t);
    void store(const std::string& target);
    void proceedIf(const std::string& condition);
    void push();
    void pop();
    void alu(const std::string& operation, const std::string& arg2);
    void alu(const std::string& operation, std::uint8_t);
    void alu(const std::string& operation);
    void aluHL(const std::string& operation);
    void bitHL(std::uint8_t);
    void clearZ();
    void switchInterrupts(bool enable, bool withDelay);
    void extraCycle();
    void forceFinish();

    std::uint8_t getOpCode() const { return m_opcode; }
    const std::string& getLabel() const { return m_label; }
    const std::vector<Op>& getOps() const { return m_ops; }
    
private:
    std::uint8_t m_opcode;
    std::string m_label;
    std::vector<Op> m_ops;
    DataType m_lastType;
};
