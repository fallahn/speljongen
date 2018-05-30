#pragma once

#include <SFML/Graphics/Texture.hpp>

#include <SFML/System/Mutex.hpp>

#include <array>
#include <cstdint>

class Display final
{
public:
    Display();
    //pixel values are 0 - 3 in classic mode
    void putPixel(std::uint8_t);
    void refresh();
    void clear(bool powerOn = false);

    const sf::Texture& getTexture() const { return m_texture; }

    void lockDisplay() { m_mutex.lock(); }
    void freeDisplay() { m_mutex.unlock(); }

private:

    sf::Texture m_texture;

    sf::Image m_imageBuffer;
    sf::Uint32 m_pixelIndex;

    sf::Mutex m_mutex;
};