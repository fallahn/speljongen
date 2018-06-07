/*
MIT License

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

#include <string>
#include <vector>

class AddressSpace;
class Disassembler final
{
public:
    Disassembler();

    void disassemble(const AddressSpace&, uint16_t start = 0, std::uint16_t end = 0xffff);
    void updateRawView(const AddressSpace&, uint16_t start = 0, std::uint16_t end = 0xffff);
    std::vector<std::uint8_t>& getRawView() { return m_rawView; }
    std::vector<std::string>& getLabels() { return m_disassembly; }

    const std::string& operator [](std::size_t i) const { return m_disassembly[i]; }

private:

    std::vector<std::string> m_disassembly;
    std::vector<std::uint8_t> m_rawView;
};
