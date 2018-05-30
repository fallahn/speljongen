#include "VramViewer.hpp"

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