#pragma once

#include <string>
#include <vector>

class AddressSpace;
class Disassembler final
{
public:
    Disassembler();

    void disassemble(const AddressSpace&, uint16_t start = 0, std::uint16_t end = 0xffff);

    const std::string& operator [](std::size_t i) const { return m_disassembly[i]; }

private:

    std::vector<std::string> m_disassembly;
};
