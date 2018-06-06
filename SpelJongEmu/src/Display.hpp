/*
MIT License

Copyright(c) 2018 Matt Marchant(Speljongen)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
    void putColourPixel(std::uint16_t);
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

    void setPixel(sf::Color);
};