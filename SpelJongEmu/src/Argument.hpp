#pragma once

#include <cstdint>
#include <string>

class Registers;
class AddressSpace;

enum class DataType
{
    D8, D16, R8
};

class Argument final
{
public:
    Argument();
    Argument(const std::string& label, std::size_t operandLength, bool memory, DataType type);

    std::size_t getOperandLength() const;
    bool isMemory() const;
    DataType getDataType() const;
    const std::string& getLabel() const;

private:
    std::string m_label;
    std::size_t m_operandLength;
    bool m_memory;
    DataType m_dataType;
};