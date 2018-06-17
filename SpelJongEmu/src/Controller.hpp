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

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <cstdint>
#include <vector>

class InterruptManager;
class Mmu;

namespace sf
{
    class Event;
}

class Controller final
{
public:
    Controller(InterruptManager&, Mmu&);

    void handleEvent(const sf::Event&);

    void tick();

    const sf::Texture& getTexture() const { return m_texture.getTexture(); }

private:
    std::uint8_t m_inputMask;
    std::uint8_t m_controllerMask;
    std::uint8_t m_lastMask;
    InterruptManager & m_interruptManager;
    Mmu& m_mmu;

    sf::RenderTexture m_texture;
    sf::Texture m_buttonTexture;
    std::vector<sf::Vertex> m_vertices;
    void updateTexture();
};