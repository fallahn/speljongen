#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

class TileAttributes;
class PixelFifo
{
public:
    virtual std::size_t length() const = 0;
    virtual void blitPixel() = 0;
    virtual void dropPixel() = 0;
    virtual void enqueue8Pixels(const std::vector<std::uint8_t>& pixels, const TileAttributes&) = 0; //TODO fixed array of 8 pixels?
    virtual void setOverlay(const std::vector<std::uint8_t>& pixelLine, std::uint16_t offset, const TileAttributes& flags, std::uint16_t oamIndex) = 0;
    virtual void clear() = 0;
};
