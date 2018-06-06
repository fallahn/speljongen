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

