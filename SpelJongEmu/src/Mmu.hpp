#pragma once

#include "AddressSpace.hpp"

#include <memory>
#include <vector>

class Mmu final : public AddressSpace
{
public:
    Mmu();

    template <typename T, typename... Args>
    T& addAddressSpace(Args&&... args)
    {
        static_assert(std::is_base_of<T, AddressSpace>::value, "Not an address space type");
        //TODO assign a single array for memory, then pass it to new address spaces
        //so that they can be addressed directly
        //http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf 
        m_spaces.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        return *m_spaces.back();
    }

    bool accepts(std::uint16_t) const override { return true; }
    void setByte(std::uint16_t, std::uint8_t) override;
    std::uint8_t getByte(std::uint16_t) const override;

private:
    std::vector<std::unique_ptr<AddressSpace>> m_spaces;
    AddressSpace& getSpace(std::uint16_t);
};