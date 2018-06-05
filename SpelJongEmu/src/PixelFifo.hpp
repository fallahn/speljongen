#pragma once

#include "IndexedQueue.hpp"

#include <cstdint>
#include <cstddef>
#include <array>

using PixelQueue = IndexedQueue<std::uint8_t, 16u>;

class TileAttributes;
class PixelFifo
{
public:
    virtual std::size_t length() const = 0;
    virtual void blitPixel() = 0;
    virtual void dropPixel() = 0;
    virtual void enqueue8Pixels(const std::array<std::uint8_t, 8>& pixels, const TileAttributes&) = 0;
    virtual void setOverlay(const std::array<std::uint8_t, 8>& pixelLine, std::uint16_t offset, const TileAttributes& flags, std::uint16_t oamIndex) = 0;
    virtual void clear() = 0;
};
