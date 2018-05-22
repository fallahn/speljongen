#pragma once

#include "Cpu.hpp"
#include "Mmu.hpp"
#include "InterruptManager.hpp"
#include "SpeedMode.hpp"
#include "Display.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

class Timer;
class Dma;
class Gpu;
class Speljongen final : public sf::Drawable
{
public:
    Speljongen();

    void start();
    void stop();

    void tick();

private:

    bool m_running;

    SpeedMode m_speedMode;
    Mmu m_mmu;
    Cpu m_cpu;
    Display m_display;

    Timer* m_timer;
    Dma* m_dma;
    Gpu* m_gpu;

    bool m_requestRefresh;
    bool m_lcdDisabled;

    void initRegisters();


    mutable sf::Text m_text;
    sf::Font m_font;
    void initRenderer();
    void updateDebug();
    void draw(sf::RenderTarget&, sf::RenderStates) const override;
};