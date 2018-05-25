#pragma once

#include "AddressSpace.hpp"

#include <memory>
#include <vector>
#include <algorithm>

//http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf 

class Mmu final : public AddressSpace
{
public:
    explicit Mmu(std::vector<std::uint8_t>&);

    bool accepts(std::uint16_t) const override;
    void setByte(std::uint16_t address, std::uint8_t value) override;
    std::uint8_t getByte(std::uint16_t address) const override;

    void addAddressSpace(AddressSpace&);

    void initBios();

private:

    std::vector<AddressSpace*> m_addressSpaces;
};