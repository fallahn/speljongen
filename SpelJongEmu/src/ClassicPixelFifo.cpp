#include "ClassicPixelFifo.hpp"
#include "Display.hpp"
#include "Lcdc.hpp"
#include "TileAttributes.hpp"

ClassicPixelFifo::ClassicPixelFifo(Display& display, Lcdc& lcdc, MemoryRegisters<GpuRegister>& registers)
    : m_display         (display),
    m_lcdc              (lcdc),
    m_memoryRegisters   (registers)
{

}

//public
std::size_t ClassicPixelFifo::length() const
{
    return m_pixels.size();
}

void ClassicPixelFifo::blitPixel()
{
    m_display.putPixel(dequeuePixel());
}

void ClassicPixelFifo::dropPixel()
{
    dequeuePixel();
}

std::uint8_t ClassicPixelFifo::dequeuePixel()
{
    m_pixelType.pop();
    auto palette = m_palettes.front();
    m_palettes.pop();

    auto pixel = m_pixels.front();
    m_pixels.pop();

    return getColour(palette, pixel);
}

void ClassicPixelFifo::enqueue8Pixels(const std::vector<std::uint8_t>& pixelLine, const TileAttributes&)
{
    for (auto p : pixelLine)
    {
        m_pixels.push(p);
        m_palettes.push(m_memoryRegisters.get(GpuRegister::registers[GpuRegister::BGP]));
        m_pixelType.push(0);
    }
}

void ClassicPixelFifo::setOverlay(const std::vector<std::uint8_t>& pixelLine, std::uint16_t offset, const TileAttributes& attribs, std::uint16_t oamIndex)
{
    bool priority = attribs.isPriority();
    std::uint8_t overlayPalette = m_memoryRegisters.get(attribs.getClassicPalette());

    for (auto j = offset; j < pixelLine.size(); ++j)
    {
        auto p = pixelLine[j];
        auto i = j - offset;
        if (m_pixelType[i] == 1)
        {
            continue;
        }

        if ((priority && m_pixels[i] == 0) || !priority && p != 0)
        {
            m_pixels[i] = p;
            m_palettes[i] = overlayPalette;
            m_pixelType[i] = 1;
        }
    }

}

const PixelQueue& ClassicPixelFifo::getPixels() const
{
    return m_pixels;
}

void ClassicPixelFifo::clear()
{
    m_pixels.clear();
    m_palettes.clear();
    m_pixelType.clear();
}

//private
std::uint8_t ClassicPixelFifo::getColour(std::uint8_t palette, std::uint8_t index) const
{
    return (palette >> (index * 2)) & 0b11;
}