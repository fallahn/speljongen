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
    m_imageBuffer.create(Width, Height, sf::Color::Red);
    m_texture.loadFromImage(m_imageBuffer);

    m_vertices[1].position = { static_cast<float>(Width), 0 };
    m_vertices[1].texCoords = m_vertices[1].position;
    m_vertices[2].position = { static_cast<float>(Width), static_cast<float>(Height) };
    m_vertices[2].texCoords = m_vertices[2].position;
    m_vertices[3].position = { 0, static_cast<float>(Height) };
    m_vertices[3].texCoords = m_vertices[3].position;
}

//public
void Display::putPixel(std::uint8_t px)
{
    assert(m_pixelIndex < MaxPixels);
    auto x = m_pixelIndex % Width;
    auto y = m_pixelIndex / Width;
    m_imageBuffer.setPixel(x , y, colours[px]);
    m_pixelIndex = (m_pixelIndex + 1) % MaxPixels;
}

void Display::requestRefresh()
{
    m_pixelIndex = 0;

    sf::Lock lock(m_mutex);
    m_texture.update(m_imageBuffer);
    
}

//private
void Display::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &m_texture;
    
    sf::Lock lock(m_mutex);
    rt.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::Quads, states);
}