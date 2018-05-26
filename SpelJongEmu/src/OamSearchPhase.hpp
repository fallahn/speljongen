#pragma once

#include "GpuPhase.hpp"
#include "MemoryRegisters.hpp"
#include "SpritePosition.hpp"
#include <array>

class Ram;
class Lcdc;
class GpuRegister;
class OamSearchPhase final : public GpuPhase
{
public:
    OamSearchPhase(Ram& oamRam, Lcdc& lcdc, MemoryRegisters& registers);

    void start();
    bool tick() override;

    std::array<SpritePosition, 10> getSprites() const { return m_sprites; }
    
private:
    enum State
    {
        ReadingX, ReadingY
    }m_state;

    Ram& m_oamRam;
    Lcdc& m_lcdc;
    MemoryRegisters&  m_registers;
    std::array<SpritePosition, 10> m_sprites;
    std::uint8_t m_spritePositionX;
    std::uint8_t m_spritePositionY;
    std::uint16_t m_index;
    std::size_t m_spritePosIndex;

    bool between(std::uint8_t from, std::uint8_t x, std::uint8_t to);
};