#pragma once

#include "AddressSpace.hpp"

class Mbc1 final : public AddressSpace
{
public: 
    Mbc1(std::vector<std::uint8_t>&, const std::vector<char>&, std::int32_t, std::int32_t);

    bool accepts(std::uint16_t) const override;

    void setByte(std::uint16_t, std::uint8_t) override;

    std::uint8_t getByte(std::uint16_t) const override;

private:

    std::vector<std::uint8_t> m_cartidgeData;
    std::vector<std::uint8_t> m_ram;
    std::int32_t m_romBanks;
    std::int32_t m_ramBanks;
    bool m_multicart;
    bool m_writeEnabled;

    mutable std::int32_t m_cachedAtZero;
    mutable std::int32_t m_cachedAtForty;

    std::int32_t m_selectedRamBank;
    std::int32_t m_selectedRomBank;
    std::int32_t m_memoryModel;

    std::uint8_t getRomByte(std::int32_t bank, std::uint16_t address) const;
    std::int32_t getRomBankForZero() const;
    std::int32_t getRomBankForForty() const;
    std::uint16_t getRamAddress(std::uint16_t) const;
};