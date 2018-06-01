#include "Disassembler.hpp"
#include "AddressSpace.hpp"
#include "OpCodesImpl.hpp"

#include <sstream>
#include <iomanip>
#include <iostream>

Disassembler::Disassembler()
    : m_disassembly(0x10000)
{

}

//public
void Disassembler::disassemble(const AddressSpace& addressSpace, std::uint16_t start, std::uint16_t end)
{
    std::uint16_t address = start;
    
    while (addressSpace.accepts(address) && address < 0xff4c)
    {      
        auto byte = addressSpace.getByte(address);
        
        OpCode currentOp;
        if (byte == 0xCB)
        {
            //ext command
            std::stringstream ss;
            ss << "0x" << std::hex << std::setfill('0') << std::setw(4);
            ss << address << ": 0xCB";
            m_disassembly[address] = ss.str();
            address++;
            byte = addressSpace.getByte(address);
            currentOp = OpCodes::ExtCommands[byte];
        }
        else
        {
            currentOp = OpCodes::Commands[byte];
        }

        std::stringstream ss;
        ss << "0x" << std::hex << std::setfill('0') << std::setw(4) << address << ": " << currentOp.getLabel();

        //TODO we need to decode the op to fetch the correct params
        auto len = currentOp.getOperandLength();
        
        m_disassembly[address++] = ss.str();

        //fill in spaces where operands live
        for (auto i = 0u; i < len; ++i)
        {
            std::stringstream ss2;
            ss2 << "0x" << std::hex << std::setfill('0') << std::setw(4) << address << ": ";
            ss2 << "0x" << std::setfill('0') << std::setw(2) << (int)addressSpace.getByte(address);
            m_disassembly[address++] = ss2.str();
        }        
    }
}
