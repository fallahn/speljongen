#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>

#include <SFML/System/Mutex.hpp>

#include <array>

class VramViewer final : public sf::Drawable, public sf::Transformable
{
public:
    VramViewer();

    void setPixel(sf::Uint32, sf::Uint32, std::uint8_t);

    void update();

private:

    sf::Image m_imageBuffer;
    sf::Texture m_texture;
    std::array<sf::Vertex, 4u> m_vertices;

    mutable sf::Mutex m_mutex;

    void draw(sf::RenderTarget&, sf::RenderStates) const override;
};

