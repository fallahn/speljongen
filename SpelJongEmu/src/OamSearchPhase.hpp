#pragma once

#include "GpuPhase.hpp"
#include "MemoryRegisters.hpp"

#include <array>

struct SpritePosition final
{
    SpritePosition(std::uint16_t x = 0, std::uint16_t y = 0, std::uint16_t address = 0)
        : m_x(x), m_y(y), m_address(address) {}

    std::uint16_t getX() const { return m_x; }
    std::uint16_t getY() const { return m_y; }
    std::uint16_t getAddress() const { return m_address; }

private:
    std::uint16_t m_x;
    std::uint16_t m_y;
    std::uint16_t m_address;
};

class Ram;
class Lcdc;
class GpuRegister;
class OamSearchPhase final : public GpuPhase
{
public:
    OamSearchPhase(Ram& oamRam, Lcdc& lcdc, MemoryRegisters<GpuRegister>& registers);

    void start();
    bool tick() override;

    const std::array<SpritePosition, 10>& getSprites() const { return m_sprites; }
    
private:
    enum State
    {
        ReadingX, ReadingY
    }m_state;

    Ram& m_oamRam;
    Lcdc& m_lcdc;
    MemoryRegisters<GpuRegister>&  m_registers;
    std::array<SpritePosition, 10> m_sprites;
    std::uint8_t m_spritePositionX;
    std::uint8_t m_spritePositionY;
    std::uint16_t m_index;
    std::size_t m_spritePosIndex;

    bool between(std::uint8_t from, std::uint8_t x, std::uint8_t to);
};