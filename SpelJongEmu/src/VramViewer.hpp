#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>

#include <SFML/System/Mutex.hpp>

#include <array>

class VramViewer final
{
public:
    VramViewer();

    void setPixel(sf::Uint32, sf::Uint32, std::uint8_t);

    void update();

    const sf::Texture& getTexture() const { return m_texture; }

    void lockDisplay() { m_mutex.lock(); }
    void freeDisplay() { m_mutex.unlock(); }

private:
    sf::Image m_imageBuffer;
    sf::Texture m_texture;

    sf::Mutex m_mutex;
};

