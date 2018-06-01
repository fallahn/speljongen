#pragma once

#include "AddressSpace.hpp"

#include <memory>
#include <string>

class Cartridge final : public AddressSpace
{
public:
    explicit Cartridge(std::vector<std::uint8_t>&);

    bool accepts(std::uint16_t) const override;

    void setByte(std::uint16_t, std::uint8_t) override;

    std::uint8_t getByte(std::uint16_t) const override;

    void load(const std::string&);

    const std::string& getTitle() const { return m_title; }

    const std::string& getInfo() const { return m_infoStr; }

    enum Type
    {
        Universal = 0x80,
        Colour = 0xc0,
        Classic = 0
    };

private:

    enum Info
    {
        ROM = 0x00,
        ROM_MBC1 = 0x01,
        ROM_MBC1_RAM = 0x02,
        ROM_MBC1_RAM_BATTERY = 0x03,
        ROM_MBC2 = 0x05,
        ROM_MBC2_BATTERY = 0x06,
        ROM_RAM = 0x08,
        ROM_RAM_BATTERY = 0x09,
        ROM_MMM01 = 0x0b,
        ROM_MMM01_SRAM = 0x0c,
        ROM_MMM01_SRAM_BATTERY = 0x0d,
        ROM_MBC3_TIMER_BATTERY = 0x0f,
        ROM_MBC3_TIMER_RAM_BATTERY = 0x10,
        ROM_MBC3 = 0x11,
        ROM_MBC3_RAM = 0x12,
        ROM_MBC3_RAM_BATTERY = 0x13,
        ROM_MBC5 = 0x19,
        ROM_MBC5_RAM = 0x1a,
        ROM_MBC5_RAM_BATTERY = 0x01b,
        ROM_MBC5_RUMBLE = 0x1c,
        ROM_MBC5_RUMBLE_SRAM = 0x1d,
        ROM_MBC5_RUMBLE_SRAM_BATTERY = 0x1e
    }m_info;

    bool m_colour;
    std::string m_title;
    std::string m_infoStr;
    std::int32_t m_romBanks;
    std::int32_t m_ramBanks;
    Type m_type;

    std::unique_ptr<AddressSpace> m_mbc;
};