#include "VramViewer.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <SFML/System/Lock.hpp>

namespace
{
    const sf::Uint32 Width = 128;
    const sf::Uint32 Height = 128;
    const std::array<sf::Color, 4u> colours =
    {
        sf::Color(155, 188, 15), sf::Color(139, 172, 15), sf::Color(48, 98, 48), sf::Color(15, 56, 15)
    };
}

VramViewer::VramViewer()
{
    m_imageBuffer.create(Width, Height, sf::Color::Green);
    m_texture.loadFromImage(m_imageBuffer);

    m_vertices[1].position = { static_cast<float>(Width), 0 };
    m_vertices[1].texCoords = m_vertices[1].position;
    m_vertices[2].position = { static_cast<float>(Width), static_cast<float>(Height) };
    m_vertices[2].texCoords = m_vertices[2].position;
    m_vertices[3].position = { 0, static_cast<float>(Height) };
    m_vertices[3].texCoords = m_vertices[3].position;
}

//public
void VramViewer::setPixel(sf::Uint32 x, sf::Uint32 y, std::uint8_t colour)
{
    m_imageBuffer.setPixel(x, y, colours[colour]);
}

void VramViewer::update()
{
    sf::Lock lock(m_mutex);
    m_texture.update(m_imageBuffer);
}

//private
void VramViewer::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &m_texture;

    sf::Lock lock(m_mutex);
    rt.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::Quads, states);
}