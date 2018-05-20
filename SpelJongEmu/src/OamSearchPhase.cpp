#include "OamSearchPhase.hpp"
#include "Lcdc.hpp"
#include "Ram.hpp"
#include "GpuRegister.hpp"

OamSearchPhase::OamSearchPhase(Ram& oamRam, Lcdc& lcdc, MemoryRegisters<GpuRegister>& registers)
    : m_oamRam          (oamRam),
    m_lcdc              (lcdc),
    m_registers         (registers),
    m_index             (0),
    m_spritePosIndex    (0)
{

}

//public
void OamSearchPhase::start()
{
    m_spritePosIndex = 0;
    m_state = State::ReadingY;
    m_spritePositionX = 0;
    m_spritePositionY = 0;
    m_index = 0;
    for (auto& sprite : m_sprites)
    {
        sprite = {};
    }
}

bool OamSearchPhase::tick()
{
    std::uint16_t spriteAddress = 0xfe00 + 4 * m_index;
    switch (m_state)
    {
    default: break;
    case ReadingX:
        m_spritePositionX = m_oamRam.getByte(spriteAddress + 1);
        if (m_spritePosIndex < m_sprites.size()
            && between(m_spritePositionY, m_registers.get(GpuRegister::registers[GpuRegister::LY]) + 16, m_spritePositionY + m_lcdc.getSpriteHeight()))
        {
            m_sprites[m_spritePosIndex++] = SpritePosition(m_spritePositionX, m_spritePositionY, spriteAddress);
        }

        m_index++;
        m_state = State::ReadingY;
        break;
    case ReadingY:
        m_spritePositionY = m_oamRam.getByte(spriteAddress);
        m_state = State::ReadingX;
        break;
    }
    return m_index < 40;
}

//private
bool OamSearchPhase::between(std::uint8_t from, std::uint8_t x, std::uint8_t to)
{
    return from <= x && x < to;
}