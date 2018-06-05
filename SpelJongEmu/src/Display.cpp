#include "Display.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Lock.hpp>

#include <cassert>
#include <iostream>

namespace
{
    const sf::Uint32 Width = 160;
    const sf::Uint32 Height = 144;
    constexpr sf::Uint32 MaxPixels = Width * Height;
    const std::array<sf::Color, 4u> colours = 
    {
        sf::Color(155, 188, 15), sf::Color(139, 172, 15), sf::Color(48, 98, 48), sf::Color(15, 56, 15)
    };
}

Display::Display()
    : m_pixelIndex(0)
{
    m_imageBuffer.create(Width, Height, colours[3]);
    m_texture.loadFromImage(m_imageBuffer);
}

//public
void Display::putPixel(std::uint8_t px)
{
    setPixel(colours[px]);
}

void Display::putColourPixel(std::uint16_t value)
{
    sf::Uint8 r(value & 0x1f);
    sf::Uint8 g((value >> 5) & 0x1f);
    sf::Uint8 b((value >> 10) & 0x1f);
    setPixel({ sf::Uint8(r * 8), sf::Uint8(g * 8), sf::Uint8(b * 8) });
}

void Display::refresh()
{
    m_pixelIndex = 0;

    sf::Lock lock(m_mutex);
    m_texture.update(m_imageBuffer);   
}

void Display::clear(bool powerOn)
{
    sf::Lock lock(m_mutex);
    m_imageBuffer.create(Width, Height, powerOn ? colours[0] : colours[3]);
    m_texture.update(m_imageBuffer);
}

//private
void Display::setPixel(sf::Color colour)
{
    assert(m_pixelIndex < MaxPixels);
    auto x = m_pixelIndex % Width;
    auto y = m_pixelIndex / Width;
    m_imageBuffer.setPixel(x, y, colour);
    m_pixelIndex = (m_pixelIndex + 1) % MaxPixels;
}