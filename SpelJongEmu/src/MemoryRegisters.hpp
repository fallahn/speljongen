#pragma once

#include "AddressSpace.hpp"
#include "GpuRegister.hpp"

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
            assert(m_registers.find(r.getAddress()) == m_registers.end());
            m_registers.insert(std::make_pair(r.getAddress(), r));
            //m_values.insert(std::make_pair(r.getAddress(), 0));
        }
    }
    std::uint8_t get(const T& r) const
    {
        auto ret = m_registers.find(r.getAddress());
        assert(ret != m_registers.end());
        return ret->second.value;
    }
    void put(const T& r, std::uint8_t value)
    {
        assert(m_registers.find(r.getAddress()) != m_registers.end());
       m_registers[r.getAddress()].value = value;
    }

    MemoryRegisters<T> freeze() const
    {
        return *this;
    }
    std::uint8_t preIncrement(const T& r)
    {
        assert(m_registers.find(r.getAddress()) != m_registers.end());
        uint8_t value = ++m_registers[r.getAddress()].value;
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
            m_registers[address].value = value;
        }
    }
    std::uint8_t getByte(std::uint16_t address) const override
    {
        assert(accepts(address));
        auto result = m_registers.find(address);
        if (result->second.getType() & Register::R)
        {
            return result->second.value;
        }

        return 0xff;
    }
private:
    //TODO we want to see where this is used and preferably
    //map to the shared memory space in the MMU
    std::map<std::uint16_t, T> m_registers;
    //std::map<std::uint16_t, std::uint8_t> m_values;

};