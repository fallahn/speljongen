#pragma once

#include "AddressSpace.hpp"

#include <memory>
#include <vector>

//http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf 

class Mmu final : public AddressSpace
{
public:
    Mmu() : m_storage(0xffff) {}

    template <typename T, typename... Args>
    T& addAddressSpace(Args&&... args)
    {
        static_assert(std::is_base_of<T, AddressSpace>::value, "Not an address space type");
        m_spaces.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        m_spaces.back()->setStorage(m_storage);
        return *m_spaces.back();
    }

    bool accepts(std::uint16_t) const override { return true; }
    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        m_storage[address] = value;
    }
    std::uint8_t getByte(std::uint16_t address) const override
    {
        return m_storage[address];
    }

private:
    std::vector<std::unique_ptr<AddressSpace>> m_spaces;
    //AddressSpace& getSpace(std::uint16_t);
    std::vector<std::uint8_t> m_storage;
};