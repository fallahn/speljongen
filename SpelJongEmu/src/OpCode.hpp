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