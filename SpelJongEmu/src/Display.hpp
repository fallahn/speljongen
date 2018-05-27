#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <SFML/System/Mutex.hpp>

#include <array>
#include <cstdint>

class Display final : public sf::Drawable, public sf::Transformable
{
public:
    Display();
    //pixel values are 0 - 3 in classic mode
    void putPixel(std::uint8_t);
    void requestRefresh();
    void waitForRefresh();
    void enableLCD();
    void disableLCD();

private:
    std::array<sf::Vertex, 4u> m_vertices;
    sf::Texture m_texture;

    sf::Image m_imageBuffer;
    sf::Uint32 m_pixelIndex;

    sf::Mutex mutable m_mutex;

    void draw(sf::RenderTarget&, sf::RenderStates) const override;
};