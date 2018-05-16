#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <cassert>

#include "Registers.hpp"
#include "AddressSpace.hpp"
#include "BitUtil.hpp"

enum class DataType
{
    D8, D16, R8, None
};

class Argument
{
public:
    enum ID
    {
        A, B, C, D, E, H, L, AF, BC, DE, HL, SP, PC, D8, D16, R8, A16,
        _BC, _DE, _HL, _A8, _A16, _C, Count
    };

    Argument(const std::string& label, std::size_t operandLength, bool memory, DataType type);

    std::size_t getOperandLength() const;
    bool isMemory() const;
    DataType getDataType() const;
    const std::string& getLabel() const;

    virtual std::uint16_t read(const Registers&, const AddressSpace&, const std::array<std::uint8_t, 2>&) = 0;
    virtual void write(Registers&, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) = 0;
    virtual std::int32_t id() const = 0;
private:
    std::string m_label;
    std::size_t m_operandLength;
    bool m_memory;
    DataType m_dataType;
};

class ArgumentA final : public Argument
{
public:
    ArgumentA() : Argument("A", 0, false, DataType::D8) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getA();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setA(static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::A; }
};

class ArgumentB final : public Argument
{
public:
    ArgumentB() : Argument("B", 0, false, DataType::D8) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getB();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setB(static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::B; }
};

class ArgumentC final : public Argument
{
public:
    ArgumentC() : Argument("C", 0, false, DataType::D8) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getC();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setC(static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::C; }
};

class ArgumentD final : public Argument
{
public:
    ArgumentD() : Argument("D", 0, false, DataType::D8) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getD();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setD(static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::D; }
};

class ArgumentE final : public Argument
{
public:
    ArgumentE() : Argument("E", 0, false, DataType::D8) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getE();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setE(static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::E; }
};

class ArgumentH final : public Argument
{
public:
    ArgumentH() : Argument("H", 0, false, DataType::D8) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getH();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setH(static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::H; }
};

class ArgumentL final : public Argument
{
public:
    ArgumentL() : Argument("L", 0, false, DataType::D8) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getH();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setH(static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::L; }
};

class ArgumentAF final : public Argument
{
public:
    ArgumentAF() : Argument("AF", 0, false, DataType::D16) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getAF();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setAF(value);
    }
    std::int32_t id() const { return Argument::AF; }
};

class ArgumentBC final : public Argument
{
public:
    ArgumentBC() : Argument("BC", 0, false, DataType::D16) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getBC();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setBC(value);
    }
    std::int32_t id() const { return Argument::BC; }
};

class ArgumentDE final : public Argument
{
public:
    ArgumentDE() : Argument("DE", 0, false, DataType::D16) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getDE();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setDE(value);
    }
    std::int32_t id() const { return Argument::DE; }
};

class ArgumentHL final : public Argument
{
public:
    ArgumentHL() : Argument("HL", 0, false, DataType::D16) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getHL();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setHL(value);
    }
    std::int32_t id() const { return Argument::HL; }
};

class ArgumentSP final : public Argument
{
public:
    ArgumentSP() : Argument("SP", 0, false, DataType::D16) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getSP();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setSP(value);
    }
    std::int32_t id() const { return Argument::SP; }
};

class ArgumentPC final : public Argument
{
public:
    ArgumentPC() : Argument("PC", 0, false, DataType::D16) {}
    std::uint16_t read(const Registers& registers, const AddressSpace&, const std::array<std::uint8_t, 2>&) override
    {
        return registers.getPC();
    }
    void write(Registers& registers, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        registers.setPC(value);
    }
    std::int32_t id() const { return Argument::PC; }
};

class ArgumentD8 final : public Argument
{
public:
    ArgumentD8() : Argument("d8", 1, false, DataType::D8) {}
    std::uint16_t read(const Registers&, const AddressSpace&, const std::array<std::uint8_t, 2>& args) override
    {
        return args[0];
    }
    void write(Registers&, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        assert(false);
    }
    std::int32_t id() const { return Argument::D8; }
};

class ArgumentD16 final : public Argument
{
public:
    ArgumentD16() : Argument("d16", 2, false, DataType::D16) {}
    std::uint16_t read(const Registers&, const AddressSpace&, const std::array<std::uint8_t, 2>& args) override
    {
        return BitUtil::toWord(args);
    }
    void write(Registers&, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        assert(false);
    }
    std::int32_t id() const { return Argument::D16; }
};

class ArgumentR8 final : public Argument
{
public:
    ArgumentR8() : Argument("r8", 1, false, DataType::R8) {}
    std::uint16_t read(const Registers&, const AddressSpace&, const std::array<std::uint8_t, 2>& args) override
    {
        return BitUtil::toSigned(args[0]);
    }
    void write(Registers&, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        assert(false);
    }
    std::int32_t id() const { return Argument::R8; }
};

class ArgumentA16 final : public Argument
{
public:
    ArgumentA16() : Argument("a16", 2, false, DataType::D16) {}
    std::uint16_t read(const Registers&, const AddressSpace&, const std::array<std::uint8_t, 2>& args) override
    {
        return BitUtil::toWord(args);
    }
    void write(Registers&, AddressSpace&, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        assert(false);
    }
    std::int32_t id() const { return Argument::A16; }
};

class Argument_BC final : public Argument
{
public:
    Argument_BC() : Argument("(BC)", 0, true, DataType::D8) {}
    std::uint16_t read(const Registers& registers, const AddressSpace& addressSpace, const std::array<std::uint8_t, 2>&) override
    {
        return addressSpace.getByte(registers.getBC());
    }
    void write(Registers& registers, AddressSpace& addressSpace, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        addressSpace.setByte(registers.getBC(), static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::_BC; }
};

class Argument_DE final : public Argument
{
public:
    Argument_DE() : Argument("(DE)", 0, true, DataType::D8) {}
    std::uint16_t read(const Registers& registers, const AddressSpace& addressSpace, const std::array<std::uint8_t, 2>&) override
    {
        return addressSpace.getByte(registers.getDE());
    }
    void write(Registers& registers, AddressSpace& addressSpace, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        addressSpace.setByte(registers.getDE(), static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::_DE; }
};

class Argument_HL final : public Argument
{
public:
    Argument_HL() : Argument("(HL)", 0, true, DataType::R8) {}
    std::uint16_t read(const Registers& registers, const AddressSpace& addressSpace, const std::array<std::uint8_t, 2>&) override
    {
        return addressSpace.getByte(registers.getHL());
    }
    void write(Registers& registers, AddressSpace& addressSpace, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        addressSpace.setByte(registers.getHL(), static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::_HL; }
};

class Argument_A8 final : public Argument
{
public:
    Argument_A8() : Argument("(a8)", 1, true, DataType::D8) {}
    std::uint16_t read(const Registers&, const AddressSpace& addressSpace, const std::array<std::uint8_t, 2>& args) override
    {
        return addressSpace.getByte(0xff00 | args[0]);
    }
    void write(Registers&, AddressSpace& addressSpace, const std::array<std::uint8_t, 2>& args, std::uint16_t value) override
    {
        addressSpace.setByte((0xff00 | args[0]), static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::_A8; }
};

class Argument_A16 final : public Argument
{
public:
    Argument_A16() : Argument("(a16)", 2, true, DataType::D8) {}
    std::uint16_t read(const Registers&, const AddressSpace& addressSpace, const std::array<std::uint8_t, 2>& args) override
    {
        return addressSpace.getByte(BitUtil::toWord(args));
    }
    void write(Registers&, AddressSpace& addressSpace, const std::array<std::uint8_t, 2>& args, std::uint16_t value) override
    {
        addressSpace.setByte(BitUtil::toWord(args), static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::_A16; }
};

class Argument_C final : public Argument
{
public:
    Argument_C() : Argument("(C)", 0, true, DataType::D8) {}
    std::uint16_t read(const Registers& registers, const AddressSpace& addressSpace, const std::array<std::uint8_t, 2>&) override
    {
        return addressSpace.getByte((0xff00 | registers.getC()));
    }
    void write(Registers& registers, AddressSpace& addressSpace, const std::array<std::uint8_t, 2>&, std::uint16_t value) override
    {
        addressSpace.setByte((0xff00 | registers.getC()), static_cast<std::uint8_t>(value));
    }
    std::int32_t id() const { return Argument::_C; }
};