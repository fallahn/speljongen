#pragma once

#include "MemoryRegisters.hpp"
#include "TileAttributes.hpp"
#include "SpritePosition.hpp"

#include <array>
#include <vector>

using PixelLine = std::array<std::uint8_t, 8>;

class PixelFifo;
class Ram;
class Lcdc;

class Fetcher final
{
public:
    Fetcher(PixelFifo&, Ram&, Ram&, Ram&, Lcdc&, MemoryRegisters&, bool);

    void init();
    void startFetching(std::uint16_t, std::uint16_t, std::uint8_t, bool, std::uint8_t);
    void disableFetching();
    void addSprite(SpritePosition, std::uint8_t, std::uint16_t);
    void tick();
    
    bool spriteInProgress() const;

    PixelLine zip(std::uint8_t, std::uint8_t, bool);
    static PixelLine zip(std::uint8_t, std::uint8_t, bool, PixelLine&);

private:
    PixelFifo& m_fifo;
    Ram& m_videoRam0;
    Ram& m_videoRam1;
    Ram& m_oamRam;
    Lcdc& m_lcdc;
    MemoryRegisters& m_registers;
    bool m_colour;
    PixelLine m_pixelLine = {};

    enum State
    {
        ReadTileID = 1,
        ReadData1 = 2,
        ReadData2 = 4,
        Push = 8,
        ReadSpriteTileID = 16,
        ReadSpriteFlags = 32,
        ReadSpriteData1 = 64,
        ReadSpriteData2 = 128,
        PushSprite = 256,
        SpriteInProgress = ReadSpriteTileID | ReadSpriteFlags | ReadSpriteData1 | ReadSpriteData2 | PushSprite
    }m_state;

    bool m_fetchingDisabled;
    std::uint16_t m_mapAddress;
    std::uint8_t m_xOffset;
    std::uint16_t m_tileDataAddress;
    bool m_tileIDSigned;
    std::uint8_t m_tileLine;
    std::uint8_t m_tileID;
    TileAttributes m_tileAttributes;
    std::uint8_t m_tileData1;
    std::uint8_t m_tileData2;
    std::uint8_t m_spriteTileLine;
    SpritePosition m_sprite;
    TileAttributes m_spriteAttributes;
    std::uint8_t m_spriteOffset;
    std::uint16_t m_spriteOamIndex;
    std::uint8_t m_divisor;

    std::uint8_t getTileData(std::uint8_t, std::uint8_t, std::uint8_t, std::uint16_t, bool, const TileAttributes&, std::uint8_t);
};
