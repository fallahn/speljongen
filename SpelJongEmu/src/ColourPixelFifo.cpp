#include "ColourPixelFifo.hpp"
#include "Lcdc.hpp"
#include "Display.hpp"
#include "ColourPalette.hpp"
#include "TileAttributes.hpp"

ColourPixelFifo::ColourPixelFifo(Lcdc& lcdc, Display& display, ColourPalette& background, ColourPalette& sprites)
    : m_lcdc            (lcdc),
    m_display           (display),
    m_backgroundPalette (background),
    m_spritePalette     (sprites)
{

}

//public
std::size_t ColourPixelFifo::length() const
{
    return m_pixels.size();
}

void ColourPixelFifo::blitPixel()
{
    m_display.putColourPixel(dequeuePixel());
}

void ColourPixelFifo::dropPixel()
{
    dequeuePixel();
}

void ColourPixelFifo::enqueue8Pixels(const std::array<std::uint8_t, 8u>& pixels, const TileAttributes& attribs)
{
    for (auto p : pixels)
    {
        m_pixels.push(p);
        m_palettes.push(attribs.getColourPaletteIndex());
        m_priorities.push(attribs.isPriority() ? 100 : -1);
    }
}

void ColourPixelFifo::setOverlay(const std::array<std::uint8_t, 8>& pixelLine, std::uint16_t offset, const TileAttributes& flags, std::uint16_t oamIndex)
{
    for (auto j = 0u; j < pixelLine.size(); ++j)
    {
        auto p = pixelLine[j];
        if (p == 0)
        {
            //skip for transparency
            continue;
        }

        auto i = j - offset;
        auto oldPriority = m_priorities[i];

        bool put = false;
        if ((oldPriority == -1 || oldPriority == 100)
            && !m_lcdc.isBgAndWindowDisplay())
        {
            put = true;
        }
        else if (oldPriority == 100)
        {
            put = (m_pixels[i] == 0);
        }
        else if (oldPriority == -1 && !flags.isPriority())
        {
            put = true;
        }
        else if (oldPriority == -1 && flags.isPriority() && m_pixels[i] == 0)
        {
            put = true;
        }
        else if (oldPriority >= 0 && oldPriority < 10)
        {
            put = (oldPriority > oamIndex);
        }

        if (put)
        {
            m_pixels[i] = p;
            m_palettes[i] = flags.getColourPaletteIndex();
            m_priorities[i] = static_cast<std::int8_t>(oamIndex);
        }
    }
}

void ColourPixelFifo::clear()
{
    m_pixels.clear();
    m_palettes.clear();
    m_priorities.clear();
}

//private
std::uint16_t ColourPixelFifo::dequeuePixel()
{
    auto priority = m_priorities.front();
    m_priorities.pop();

    auto palette = m_palettes.front();
    m_palettes.pop();

    auto pixel = m_pixels.front();
    m_pixels.pop();

    return getColour(priority, palette, pixel);
}

std::uint16_t ColourPixelFifo::getColour(std::int8_t priority, std::uint8_t palette, std::uint8_t colour)
{
    if (priority >= 0 && priority < 10)
    {
        return m_spritePalette.getPalette(palette)[colour];
    }
    return m_backgroundPalette.getPalette(palette)[colour];
}