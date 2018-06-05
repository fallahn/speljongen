#include "Fetcher.hpp"
#include "PixelFifo.hpp"
#include "Ram.hpp"
#include "Lcdc.hpp"
#include "BitUtil.hpp"

namespace
{
    const PixelLine EmptyPixelLine = {};
    const TileAttributes EmptyTileAttribs;
}

Fetcher::Fetcher(Ram& ram0, Ram& ram1, Ram& oam, Lcdc& lcdc, MemoryRegisters& registers)
    : m_fifo            (nullptr),
    m_videoRam0         (ram0),
    m_videoRam1         (ram1),
    m_oamRam            (oam),
    m_lcdc              (lcdc),
    m_registers         (registers),
    m_colour            (false),
    m_state             (ReadTileID),
    m_fetchingDisabled  (false),
    m_mapAddress        (0),
    m_xOffset           (0),
    m_tileDataAddress   (0),
    m_tileIDSigned      (false),
    m_tileLine          (0),
    m_tileID            (0),
    m_tileData1         (0),
    m_tileData2         (0),
    m_spriteTileLine    (0),
    m_spriteOffset      (0),
    m_spriteOamIndex    (0),
    m_divisor           (2)
{

}

//public
void Fetcher::init()
{
    m_state = ReadTileID;
    m_tileID = 0;
    m_tileData1 = 0;
    m_tileData2 = 0;
    m_divisor = 2;
    m_fetchingDisabled = false;
}

void Fetcher::startFetching(std::uint16_t mapAddress, std::uint16_t tileDataAddress, std::uint8_t xOffset, bool tileIDSigned, std::uint8_t tileLine)
{
    m_mapAddress = mapAddress;
    m_tileDataAddress = tileDataAddress;
    m_xOffset = xOffset;
    m_tileIDSigned = tileIDSigned;
    m_tileLine = tileLine;
    m_fifo->clear();

    m_state = ReadTileID;
    m_tileID = 0;
    m_tileData1 = 0;
    m_tileData2 = 0;
    m_divisor = 2;
}

void Fetcher::disableFetching()
{
    m_fetchingDisabled = true;
}

void Fetcher::addSprite(SpritePosition sprite, std::uint8_t offset, std::uint16_t oamIndex)
{
    m_sprite = sprite;
    m_state = ReadSpriteTileID;
    m_spriteTileLine = m_registers.getByte(MemoryRegisters::LY) + 16 - sprite.getY();
    m_spriteOffset = offset;
    m_spriteOamIndex = oamIndex;
}

void Fetcher::tick()
{
    if (m_fetchingDisabled && m_state == ReadTileID)
    {
        if (m_fifo->length() <= 8)
        {
            m_fifo->enqueue8Pixels(EmptyPixelLine, m_tileAttributes);
        }
        return;
    }

    //count cycles
    if (--m_divisor == 0)
    {
        m_divisor = 2;
    }
    else
    {
        return;
    }

    switch (m_state)
    {
    default: break;
    case ReadTileID:
        m_tileID = m_videoRam0.getByte(m_mapAddress + m_xOffset);
        if (m_colour)
        {
            m_tileAttributes = TileAttributes::valueOf(m_videoRam1.getByte(m_mapAddress + m_xOffset));
        }
        else
        {
            m_tileAttributes = EmptyTileAttribs;
        }
        m_state = ReadData1;
        break;
    case ReadData1:
        m_tileData1 = getTileData(m_tileID, m_tileLine, 0, m_tileDataAddress, m_tileIDSigned, m_tileAttributes, 8);
        m_state = ReadData2;
        break;
    case ReadData2:
        m_tileData2 = getTileData(m_tileID, m_tileLine, 1, m_tileDataAddress, m_tileIDSigned, m_tileAttributes, 8);
        m_state = Push;
        break;
    case Push:
        if (m_fifo->length() <= 8)
        {
            m_fifo->enqueue8Pixels(zip(m_tileData1, m_tileData2, m_tileAttributes.isXFlip()), m_tileAttributes);
            m_xOffset = (m_xOffset + 1) % 0x20;
            m_state = ReadTileID;
        }
        break;
    case ReadSpriteTileID:
        m_tileID = m_oamRam.getByte(m_sprite.getAddress() + 2);
        m_state = ReadSpriteFlags;
        break;
    case ReadSpriteFlags:
    {
        std::uint8_t b = m_oamRam.getByte(m_sprite.getAddress() + 3);
        m_spriteAttributes = TileAttributes::valueOf(b);
        m_state = ReadSpriteData1;
    }
        break;
    case ReadSpriteData1:
        if (m_lcdc.getSpriteHeight() == 16)
        {
            m_tileID &= 0xfe;
        }
        m_tileData1 = getTileData(m_tileID, m_spriteTileLine, 0, 0x8000, false, m_spriteAttributes, m_lcdc.getSpriteHeight());
        m_state = ReadSpriteData2;
        break;
    case ReadSpriteData2:
        m_tileData2 = getTileData(m_tileID, m_spriteTileLine, 1, 0x8000, false, m_spriteAttributes, m_lcdc.getSpriteHeight());
        m_state = PushSprite;
        break;
    case PushSprite:
        m_fifo->setOverlay(zip(m_tileData1, m_tileData2, m_spriteAttributes.isXFlip()), m_spriteOffset, m_spriteAttributes, m_spriteOamIndex);
        m_state = ReadTileID;
        break;
    }
}

bool Fetcher::spriteInProgress() const
{
    return (m_state & SpriteInProgress) != 0;
}

PixelLine Fetcher::zip(std::uint8_t data1, std::uint8_t data2, bool reverse)
{
    return zip(data1, data2, reverse, m_pixelLine);
}

PixelLine Fetcher::zip(std::uint8_t data1, std::uint8_t data2, bool reverse, PixelLine& line)
{
    for (auto i = 7; i >= 0; --i)
    {
        std::uint8_t mask = (1 << i);
        std::uint8_t p = 2 * ((data2 & mask) == 0 ? 0 : 1) + ((data1 & mask) == 0 ? 0 : 1);
        if (reverse)
        {
            line[i] = p;
        }
        else
        {
            line[7 - i] = p;
        }
    }
    return line;
}

//private
std::uint8_t Fetcher::getTileData(std::uint8_t tileID, std::uint8_t tileLine, std::uint8_t byteNumber,
    std::uint16_t tileAddress, bool isSigned, const TileAttributes& attributes, std::uint8_t tileHeight)
{
    //hmm the original source has some dubious shadowing going on
    //with both the parameter name and member name being tileDataAddress.
    //this assumes the original refers to the local (parameter) value, and renames it tileAddress
    std::uint16_t effectiveLine = 0;
    if (attributes.isYFlip())
    {
        effectiveLine = tileHeight - 1 - tileLine;
    }
    else
    {
        effectiveLine = tileLine;
    }

    std::int32_t address = 0;
    if (isSigned)
    {
        address = tileAddress + BitUtil::toSigned(tileID) * 0x10;
    }
    else
    {
        address = tileAddress + tileID * 0x10;
    }
    
    Ram& videoRam = (attributes.getBank() == 0 || !m_colour) ? m_videoRam0 : m_videoRam1;
    return videoRam.getByte(address + effectiveLine * 2 + byteNumber);
}