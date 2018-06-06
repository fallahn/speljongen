#pragma once

#include "AddressSpace.hpp"

class Mbc5 final : public AddressSpace
{
public:
    Mbc5(std::vector<std::uint8_t>&, const std::vector<char>&, std::int32_t, std::int32_t);

    std::string getLabel() const override { return "MBC 5"; }

    bool accepts(std::uint16_t) const override;

    void setByte(std::uint16_t, std::uint8_t) override;

    std::uint8_t getByte(std::uint16_t) const override;

private:
    std::vector<std::uint8_t> m_cartridgeData;
    std::vector<std::uint8_t> m_ram;
    std::int32_t m_romBanks;
    std::int32_t m_ramBanks;

    std::int32_t m_selectedRomBank;
    std::int32_t m_selectedRamBank;

    bool m_ramWriteEnabled;

    std::uint8_t getRomByte(std::int32_t bank, std::uint16_t address) const;
    std::uint16_t getRamAddress(std::uint16_t address) const;
};