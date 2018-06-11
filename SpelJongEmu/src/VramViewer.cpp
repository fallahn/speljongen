#include "VramViewer.hpp"
#include "Mmu.hpp"
#include "MemoryRegisters.hpp"

#include <SFML/System/Lock.hpp>

namespace
{
    const sf::Uint32 Width = 128;
    const sf::Uint32 Height = 192;
    const std::array<sf::Color, 4u> colours =
    {
        sf::Color(255, 255, 255), sf::Color(77, 77, 77), sf::Color(38, 38, 38), sf::Color(0, 0, 0)
    };
}

VramViewer::VramViewer(const Mmu& mmu)
    : m_mmu(mmu)
{
    m_imageBuffer.create(Width, Height, sf::Color::Black);
    m_texture.loadFromImage(m_imageBuffer);
}

//public
void VramViewer::setPixel(sf::Uint32 x, sf::Uint32 y, std::uint8_t idx)
{
    auto palette = m_mmu.getByte(MemoryRegisters::BGP);
    std::uint8_t colour = (palette >> (idx * 2)) & 0b11;

    m_imageBuffer.setPixel(x, y, colours[colour]);
}

void VramViewer::update()
{
#ifdef USE_THREADING
    sf::Lock lock(m_mutex);
#endif
    m_texture.update(m_imageBuffer);
}

//private