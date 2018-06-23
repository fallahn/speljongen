#include "OpCodesImpl.hpp"

#include <sstream>
#include <iomanip>
#include <iostream>

//enable outputting info about opcodes
//#define DUMP_OPS

namespace
{
    class IndexedList final
    {
    public:
        IndexedList(std::uint8_t start, std::uint8_t step, const std::vector<std::string>& labels)
        {
            for (std::uint8_t i = 0, j = start; i < labels.size(); ++i, j += step)
            {
                m_values.emplace_back(std::make_pair(j, labels[i]));
            }
        }

        auto begin() { return m_values.begin(); }
        auto end() { return m_values.end(); }

    private:
        std::vector<std::pair<std::uint8_t, std::string>> m_values;
    };

    std::string to_hex(std::uint16_t i)
    {
        std::stringstream stream;
        stream << "0x"
            << std::setfill('0') << std::setw(2)
            << std::hex << (int)i;
        return stream.str();
    }
}

std::vector<OpCode> OpCodes::Commands;
std::vector<OpCode> OpCodes::ExtCommands;

//public
void OpCodes::init()
{
    if (!Commands.empty()) return;

    std::vector<std::unique_ptr<OpCodeBuilder>> opcodes(0x100);
    std::vector<std::unique_ptr<OpCodeBuilder>> extOpcodes(0x100);

    regCmd(opcodes, 0x00, "NOP");
    IndexedList list(0x01, 0x10, { "BC", "DE", "HL", "SP" });
    for(const auto& pair : list)
    {
        regLoad(opcodes, pair.first, pair.second, "d16");
    }

    list = IndexedList(0x02, 0x10, { "(BC)", "(DE)" });
    for (const auto& pair : list)
    {
        regLoad(opcodes, pair.first, pair.second, "A");
    }

    list = IndexedList(0x03, 0x10, { "BC", "DE", "HL", "SP" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, "INC {}");
        builder.load(pair.second);
        builder.alu("INC");
        builder.store(pair.second);
    }

    list = IndexedList(0x04, 0x08, { "B", "C", "D", "E", "H", "L", "(HL)", "A" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, "INC {}");
        builder.load(pair.second);
        builder.alu("INC");
        builder.store(pair.second);
    }

    list = IndexedList(0x05, 0x08, { "B", "C", "D", "E", "H", "L", "(HL)", "A" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, "DEC {}");
        builder.load(pair.second);
        builder.alu("DEC");
        builder.store(pair.second);
    }

    list = IndexedList(0x06, 0x08, { "B", "C", "D", "E", "H", "L", "(HL)", "A" });
    for (const auto& pair : list)
    {
        regLoad(opcodes, pair.first, pair.second, "d8");
    }

    list = IndexedList(0x07, 0x08, { "RLC", "RRC", "RL", "RR" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, pair.second + "A");
        builder.load("A");
        builder.alu(pair.second);
        builder.clearZ();
        builder.store("A");
    }

    regLoad(opcodes, 0x08, "(a16)", "SP");

    list = IndexedList(0x09, 0x10, { "BC", "DE", "HL", "SP" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, "ADD HL, {}");
        builder.load("HL");
        builder.alu("ADD", pair.second);
        builder.store("HL");
    }

    list = IndexedList(0x0a, 0x10, { "(BC)", "(DE)" });
    for (const auto& pair : list)
    {
        regLoad(opcodes, pair.first, "A", pair.second);
    }

    list = IndexedList(0x0b, 0x10, { "BC", "DE", "HL", "SP" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, "DEC {}");
        builder.load(pair.second);
        builder.alu("DEC");
        builder.store(pair.second);
    }

    regCmd(opcodes, 0x10, "STOP");
    {
        auto& builder = regCmd(opcodes, 0x18, "JR r8");
        builder.load("PC");
        builder.alu("ADD", "r8");
        builder.store("PC");
    }

    list = IndexedList(0x20, 0x08, { "NZ", "Z", "NC", "C" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, "JR {},r8");
        builder.load("PC");
        builder.proceedIf(pair.second);
        builder.alu("ADD", "r8");
        builder.store("PC");
    }

    {
        auto& builder = regCmd(opcodes, 0x22, "LD (HL+),A");
        builder.copyByte("(HL)", "A");
        builder.aluHL("INC");
    }
    {
        auto& builder = regCmd(opcodes, 0x2a, "LD A,(HL+)");
        builder.copyByte("A", "(HL)");
        builder.aluHL("INC");
    }
    {
        auto& builder = regCmd(opcodes, 0x27, "DAA");
        builder.load("A");
        builder.alu("DAA");
        builder.store("A");
    }
    {
        auto& builder = regCmd(opcodes, 0x2f, "CPL");
        builder.load("A");
        builder.alu("CPL");
        builder.store("A");
    }
    {
        auto& builder = regCmd(opcodes, 0x32, "LD (HL-),A");
        builder.copyByte("(HL)", "A");
        builder.aluHL("DEC");
    }
    {
        auto& builder = regCmd(opcodes, 0x3a, "LD A,(HL-)");
        builder.copyByte("A", "(HL)");
        builder.aluHL("DEC");
    }
    {
        auto& builder = regCmd(opcodes, 0x37, "SCF");
        builder.load("A");
        builder.alu("SCF");
        builder.store("A");
    }
    {
        auto& builder = regCmd(opcodes, 0x3f, "CCF");
        builder.load("A");
        builder.alu("CCF");
        builder.store("A");
    }

    list = IndexedList(0x40, 0x08, { "B", "C", "D", "E", "H", "L", "(HL)", "A" });
    for (const auto& pair : list)
    {
        auto subList = IndexedList(pair.first, 0x01, { "B", "C", "D", "E", "H", "L", "(HL)", "A" });
        for (const auto& subPair : subList)
        {
            if (subPair.first == 0x76) //this is HALT
            {
                continue;
            }
            regLoad(opcodes, subPair.first, pair.second, subPair.second);
        }
    }
    regCmd(opcodes, 0x76, "HALT");

    list = IndexedList(0x80, 0x08, { "ADD", "ADC", "SUB", "SBC", "AND", "XOR", "OR", "CP" });
    for (const auto& pair : list)
    {
        auto subList = IndexedList(pair.first, 0x01, { "B", "C", "D", "E", "H", "L", "(HL)", "A" });
        for (const auto& subPair : subList)
        {
            auto& builder = regCmd(opcodes, subPair, pair.second + " {}");
            builder.load("A");
            builder.alu(pair.second, subPair.second);
            builder.store("A");
        }
    }

    list = IndexedList(0xc0, 0x08, { "NZ", "Z", "NC", "C" });
    for(const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, "RET {}");
        builder.extraCycle();
        builder.proceedIf(pair.second);
        builder.pop();
        builder.forceFinish();
        builder.store("PC");
    }

    list = IndexedList(0xc1, 0x10, { "BC", "DE", "HL", "AF" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, "POP {}");
        builder.pop();
        builder.store(pair.second);
    }

    list = IndexedList(0xc2, 0x08, { "NZ", "Z", "NC", "C" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, "JP {},a16");
        builder.load("a16");
        builder.proceedIf(pair.second);
        builder.store("PC");
        builder.extraCycle();
    }

    {
        auto& builder = regCmd(opcodes, 0xc3, "JP a16");
        builder.load("a16");
        builder.store("PC");
        builder.extraCycle();
    }

    list = IndexedList(0xc4, 0x08, { "NZ", "Z", "NC", "C" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, "CALL {},a16");
        builder.proceedIf(pair.second);
        builder.extraCycle();
        builder.load("PC");
        builder.push();
        builder.load("a16");
        builder.store("PC");
    }

    list = IndexedList(0xc5, 0x10, { "BC", "DE", "HL", "AF" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, "PUSH {}");
        builder.extraCycle();
        builder.load(pair.second);
        builder.push();
    }

    list = IndexedList(0xc6, 0x08, { "ADD", "ADC", "SUB", "SBC", "AND", "XOR", "OR", "CP" });
    for (const auto& pair : list)
    {
        auto& builder = regCmd(opcodes, pair, pair.second + " d8");
        builder.load("A");
        builder.alu(pair.second, "d8");
        builder.store("A");
    }

    for (std::uint16_t i = 0xc7, j = 0; i <= 0xf7; i += 0x10, j += 0x10)
    {
        auto& builder = regCmd(opcodes, static_cast<std::uint8_t>(i), "RST " + to_hex(j));
        builder.load("PC");
        builder.push();
        builder.forceFinish();
        builder.loadWord(j);
        builder.store("PC");
    }

    {
        auto& builder = regCmd(opcodes, 0xc9, "RET");
        builder.pop();
        builder.forceFinish();
        builder.store("PC");
    }

    {
        auto& builder = regCmd(opcodes, 0xcd, "CALL a16");
        builder.load("PC");
        builder.extraCycle();
        builder.push();
        builder.load("a16");
        builder.store("PC");
    }

    for (std::uint16_t i = 0xcf, j = 0x08; i <= 0xff; i += 0x10, j += 0x10)
    {
        auto& builder = regCmd(opcodes, static_cast<std::uint8_t>(i), "RST " + to_hex(j));
        builder.load("PC");
        builder.push();
        builder.forceFinish();
        builder.loadWord(j);
        builder.store("PC");
    }
    {
        auto& builder = regCmd(opcodes, 0xd9, "RETI");
        builder.pop();
        builder.forceFinish();
        builder.store("PC");
        builder.switchInterrupts(true, false);
    }
    regLoad(opcodes, 0xe2, "(C)", "A");
    regLoad(opcodes, 0xf2, "A", "(C)");

    {
        auto& builder = regCmd(opcodes, 0xe9, "JP (HL)");
        builder.load("HL");
        builder.store("PC");
    }

    {
        auto& builder = regCmd(opcodes, 0xe0, "LDH (a8),A");
        builder.copyByte("(a8)", "A");
    }
    {
        auto& builder = regCmd(opcodes, 0xf0, "LDH A,(a8)");
        builder.copyByte("A", "(a8)");
    }
    {
        auto& builder = regCmd(opcodes, 0xe8, "ADD SP,r8");
        builder.load("SP");
        builder.alu("ADD_SP", "r8");
        builder.extraCycle();
        builder.store("SP");
    }
    {
        auto& builder = regCmd(opcodes, 0xf8, "LD HL,SP+r8");
        builder.load("SP");
        builder.alu("ADD_SP", "r8");
        builder.store("HL");
    }
    regLoad(opcodes, 0xea, "(a16)", "A");
    regLoad(opcodes, 0xfa, "A", "(a16)");

    {
        auto& builder = regCmd(opcodes, 0xf3, "DI");
        builder.switchInterrupts(false, true);
    }
    {
        auto& builder = regCmd(opcodes, 0xfb, "EI");
        builder.switchInterrupts(true, true);
    }
    {
        auto& builder = regLoad(opcodes, 0xf9, "SP", "HL");
        builder.extraCycle();
    }

    list = IndexedList(0x00, 0x08, { "RLC", "RRC", "RL", "RR", "SLA", "SRA", "SWAP", "SRL" });
    for (const auto& pair : list)
    {
        auto subList = IndexedList(pair.first, 0x01, { "B", "C", "D", "E", "H", "L", "(HL)", "A" });
        for (const auto& subPair : subList)
        {
            auto& builder = regCmd(extOpcodes, subPair, pair.second + " {}");
            builder.load(subPair.second);
            builder.alu(pair.second);
            builder.store(subPair.second);
        }
    }

    list = IndexedList(0x40, 0x40, { "BIT", "RES", "SET" });
    for (const auto& pair : list)
    {
        for (std::uint8_t i = 0; i < 0x08; ++i)
        {
            auto subList = IndexedList(pair.first + i * 0x08, 0x01, { "B", "C", "D", "E", "H", "L", "(HL)", "A" });
            for (const auto& subPair : subList)
            {
                if(pair.second == "BIT" && subPair.second == "(HL)")
                { 
                    auto& builder = regCmd(extOpcodes, subPair, "BIT " + std::to_string(i) + ",(HL)");
                    builder.bitHL(i);
                }
                else
                {
                    auto& builder = regCmd(extOpcodes, subPair, pair.second + " " + std::to_string(i) + "," + subPair.second);
                    builder.load(subPair.second);
                    builder.alu(pair.second, i);
                    builder.store(subPair.second);
                }
            }
        }
    }

    //finally build the actual ops :3
#ifdef DUMP_OPS
    std::cout << "Opcodes:\n";
#endif
    for (auto& builder : opcodes)
    {
        if (builder)
        {
            Commands.emplace_back(*builder);
#ifdef DUMP_OPS
            std::cout << Commands.size() << ", " << std::hex << (int)Commands.back().getOpcode() << std::dec 
                << " - " << Commands.back().getLabel() << ", length: " << Commands.back().getOperandLength() << "\n";
#endif
        }
        else
        {
            Commands.emplace_back();
#ifdef DUMP_OPS
            std::cout << Commands.size() << "  NULL\n";
#endif
        }
    }
#ifdef DUMP_OPS
    std::cout << "\n\nExtended opcodes:\n";
#endif
    for (auto& builder : extOpcodes)
    {
        if (builder)
        {
            ExtCommands.emplace_back(*builder);
#ifdef DUMP_OPS
            std::cout << ExtCommands.size() << ", " << std::hex << (int)ExtCommands.back().getOpcode() << std::dec 
                << " - " << ExtCommands.back().getLabel() << ", length: " << ExtCommands.back().getOperandLength() << "\n";
#endif
        }
        else
        {
            ExtCommands.emplace_back();
#ifdef DUMP_OPS
            std::cout << ExtCommands.size() << "  NULL\n";
#endif
        }
    }
}

//private
OpCodeBuilder& OpCodes::regLoad(std::vector<std::unique_ptr<OpCodeBuilder>>& commands, std::uint8_t opcode, const std::string& target, const std::string& source)
{
    auto& builder = regCmd(commands, opcode, "LD " + target + "," + source);
    builder.copyByte(target, source);
    return builder;
}

OpCodeBuilder& OpCodes::regCmd(std::vector<std::unique_ptr<OpCodeBuilder>>& commands, std::uint8_t opcode, const std::string& label)
{
    commands[opcode] = std::make_unique<OpCodeBuilder>(opcode, label);
    return *commands[opcode];
}

OpCodeBuilder& OpCodes::regCmd(std::vector<std::unique_ptr<OpCodeBuilder>>& commands, std::pair<std::uint8_t, std::string> opcode, const std::string& label)
{
    std::string code = label;
    static const std::string token("{}");
    size_t start_pos = code.find(token);
    if (start_pos != std::string::npos)
    {
        code.replace(start_pos, token.length(), opcode.second);
    }
    return regCmd(commands, opcode.first, code);
}