#pragma once

#include "AddressSpace.hpp"

#include <memory>
#include <vector>
#include <algorithm>

//http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf 

class Mmu final : public AddressSpace
{
public:
    Mmu();

    template <typename T, typename... Args>
    T& addAddressSpace(Args&&... args)
    {
        //static_assert(std::is_base_of<T, AddressSpace>::value, "Not an address space type");
        m_spaces.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        m_spaces.back()->setStorage(m_storage);
        return *dynamic_cast<T*>(m_spaces.back().get());
    }

    template <typename T>
    T& getAddressSpace(AddressSpace::Type type)
    {
        auto result = std::find_if(m_spaces.begin(), m_spaces.end(),
            [type](const std::unique_ptr<AddressSpace>& s)
        {
            return s->type() == type;
        });
        assert(result != m_spaces.end());

        return *dynamic_cast<T*>(result->get());
    }

    bool accepts(std::uint16_t) const override;
    void setByte(std::uint16_t address, std::uint8_t value) override;
    std::uint8_t getByte(std::uint16_t address) const override;


private:
    std::vector<std::unique_ptr<AddressSpace>> m_spaces;
    std::vector<std::uint8_t> m_storage;
};