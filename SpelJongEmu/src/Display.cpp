#include "Display.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Image.hpp>

#include <cassert>
#include <iostream>

namespace
{
    const sf::Uint32 Width = 160;
    const sf::Uint32 Height = 144;
    constexpr sf::Uint32 MaxPixels = Width * Height;
    const std::array<sf::Color, 4u> colours = 
    {
        sf::Color::White, sf::Color(170, 170, 170), sf::Color(85, 85, 85), sf::Color::Black
    };
}

Display::Display()
    : m_pixelIndex(0)
{
    m_imageBuffer.create(Width, Height, sf::Color::Red);
    m_texture.loadFromImage(m_imageBuffer);

    m_vertices[1].position = { static_cast<float>(Width), 0 };
    m_vertices[2].position = { static_cast<float>(Width), static_cast<float>(Height) };
    m_vertices[3].position = { 0, static_cast<float>(Height) };
}

//public
void Display::putPixel(std::uint8_t px)
{
    assert(m_pixelIndex < MaxPixels);
    m_imageBuffer.setPixel(m_pixelIndex % Width, m_pixelIndex / Width, colours[px]);
    m_pixelIndex = (m_pixelIndex + 1) % MaxPixels;
    if (px > 0) std::cout << (int)px << "\n";
}

void Display::requestRefresh()
{
    m_texture.update(m_imageBuffer);
    m_pixelIndex = 0;
}

void Display::waitForRefresh() {}

void Display::enableLCD() {}

void Display::disableLCD() {}

//private
void Display::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &m_texture;
    rt.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::Quads, states);
}