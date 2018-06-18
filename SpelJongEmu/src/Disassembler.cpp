#include "Disassembler.hpp"
#include "AddressSpace.hpp"
#include "OpCodesImpl.hpp"

#include <sstream>
#include <iomanip>
#include <iostream>

Disassembler::Disassembler()
    : m_disassembly (0x10000),
    m_rawView       (0x10000)
{

}

//public
void Disassembler::disassemble(const AddressSpace& addressSpace, std::uint16_t start, std::uint16_t end)
{
    //TODO this isn't strictly correct - we need to make sure start is actually
    //the address of an opcode and not an operand, otherwise labels are misinterpreted.
    //could probably add a first pass function which marks all the addresses with ops/operands
    //although this won't work for RAM locations...

    //TODO this also needs to check the active ROM and RAM banks so that the correct, currently
    //mapped data is visible in the debugger

    std::uint16_t address = start;
    
    while (addressSpace.accepts(address) && address < 0xff4c && address < end)
    {      
        auto byte = addressSpace.getByte(address);
        
        std::size_t len = 0;

        OpCode currentOp;
        if (byte == 0xCB)
        {
            byte = addressSpace.getByte(address + 1);
            len++;
            currentOp = OpCodes::ExtCommands[byte];
        }
        else
        {
            currentOp = OpCodes::Commands[byte];
        }

        //TODO we need to decode the op to fetch the correct params
        m_disassembly[address++] = currentOp.getLabel();
        len += currentOp.getOperandLength(); 
        
        //fill in spaces where operands live
        for (auto i = 0u; i < len; ++i)
        {
            m_disassembly[address++].clear();
        }        
    }

    updateRawView(addressSpace, start, end);
}

void Disassembler::updateRawView(const AddressSpace& addressSpace, uint16_t start, std::uint16_t end)
{
    for (auto i = start; i < end; ++i)
    {
        m_rawView[i] = addressSpace.getByte(i);
    }
}
