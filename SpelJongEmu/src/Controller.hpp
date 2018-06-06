#pragma once

#include <cstdint>

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

private:
    std::uint8_t m_inputMask;
    std::uint8_t m_lastMask;
    InterruptManager & m_interruptManager;
    Mmu& m_mmu;
};