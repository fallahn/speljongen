#include "Display.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Lock.hpp>

#include <cassert>
#include <iostream>
#include <algorithm>

namespace
{
    const sf::Uint32 Width = 160;
    const sf::Uint32 Height = 144;
    constexpr sf::Uint32 MaxPixels = Width * Height;
    const std::array<sf::Color, 4u> colours = 
    {
        //sf::Color(155, 188, 15), sf::Color(139, 172, 15), sf::Color(48, 98, 48), sf::Color(15, 56, 15)
        //sf::Color(230, 249, 218), sf::Color(153, 200, 134), sf::Color(67, 121, 105), sf::Color(5, 31, 42)
        sf::Color(224, 248, 208), sf::Color(136, 192, 112), sf::Color(48, 104, 80), sf::Color(8, 24, 32)
    };

    //converts from given bit depth to 16 bit colour
    sf::Uint32 to16Bit(sf::Uint32 colour, sf::Uint32 sourceDepth)
    {
        while (sourceDepth < 16)
        {
            colour = (colour << sourceDepth) | colour;
            sourceDepth += sourceDepth;
        }

        if (16 < sourceDepth)
        {
            colour >>= (sourceDepth - 16);
        }
        return colour;
    }

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
    sf::Uint32 r(value & 0x1f);
    sf::Uint32 g((value >> 5) & 0x1f);
    sf::Uint32 b((value >> 10) & 0x1f);

    //this is an attempt to fix the colours
    //as direct RGB conversion often appears
    //too dark compared to a real gbc. For a 
    //copmparison look at the GBC colour modes
    //found in the BGB emulator.

    /*r = to16Bit(r, 5);
    g = to16Bit(g, 5);
    b = to16Bit(b, 5);*/

    /*r <<= 3;
    g <<= 3;
    b <<= 3;*/

    /*sf::Uint32 R = (r * 26 + g * 4 + b * 2);
    sf::Uint32 G = (g * 24 + b * 8);
    sf::Uint32 B = (r * 6 + g * 4 + b * 22);*/

    /*const sf::Uint32 minimum = 960;
    R = to16Bit(std::min(minimum, R), 10);
    G = to16Bit(std::min(minimum, G), 10);
    B = to16Bit(std::min(minimum, B), 10);*/

    sf::Uint32 c = (((r * 13 + g * 2 + b) >> 1) << 16)
                    | ((g * 3 + b) << 9)
                    | ((r * 3 + g * 2 + b * 11) >> 1);

    //sf::Uint32 c = (R << 24) | (G << 16) | (B << 8) | 255;

    setPixel(sf::Color((c << 8)| 0xff/*sf::Uint8(R << 3), sf::Uint8(G << 3), sf::Uint8(B << 3)*/));
}

void Display::refresh()
{
    m_pixelIndex = 0;
#ifdef USE_THREADING
    sf::Lock lock(m_mutex);
#endif
    m_texture.update(m_imageBuffer);   
}

void Display::clear(bool powerOn)
{
#ifdef USE_THREADING
    sf::Lock lock(m_mutex);
#endif
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