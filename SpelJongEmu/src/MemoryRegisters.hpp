#pragma once

#include "AddressSpace.hpp"

class Register
{
public:
    virtual ~Register() = default;
    virtual std::uint16_t getAddress() const = 0;
    //actually flags ie 0b01 = read, 0b10 = write etc
    enum Type
    {
        R = 1, W = 2, RW = 3
    };
    virtual Type getType() const = 0;
};

#include <map>
#include <vector>
#include <cassert>

template <class T>
class MemoryRegisters final : public AddressSpace
{
public:
    explicit MemoryRegisters(const std::vector<T>& registers)
    {
        static_assert(std::is_base_of<Register, T>::value, "");
        for (auto& r : registers)
        {
            assert(m_registers.find(r.getAddress() == m_registers.end()));
            m_registers.insert(std::make_pair(r.getAddress(), r));
            m_values.insert(std::make_pair(r.getAddress(), 0));
        }
    }
    std::uint8_t get(const T& r) const
    {
        auto ret = m_values.find(r.getAddress());
        assert(ret != m_values.end());
        return ret->second;
    }
    void put(const T& r, std::uint8_t value)
    {
        assert(m_values.find(r.getAddress()) != m_values.end());
        return m_values[r.getAddress()];
    }

    MemoryRegisters<T> freeze() const
    {
        return *this;
    }
    std::uint8_t preIncrement(const T& r)
    {
        assert(m_values.find(r.getAddress()) != m_values.end());
        uint8_t value = ++m_values[r.getAddress()];
        return value;
    }

    bool accepts(std::uint16_t address) const override
    {
        return m_registers.count(address) > 0;
    }
    void setByte(std::uint16_t address, uint8_t value) override
    {
        assert(accepts(address));
        if (m_registers[address].getType() & Register::W)
        {
            m_values[address] = value;
        }
    }
    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        if (m_registers[address].getType() & Register::R)
        {
            return m_values[address];
        }
        return 0xff;
    }
private:
    //TODO we want to see where this is used and preferably
    //map to the shared memory space in the MMU
    std::map<std::uint16_t, T> m_registers;
    std::map<std::uint16_t, std::uint8_t> m_values;

};