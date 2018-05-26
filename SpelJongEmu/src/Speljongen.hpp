#pragma once

#include "Cpu.hpp"
#include "Mmu.hpp"
#include "InterruptManager.hpp"
#include "SpeedMode.hpp"
#include "Display.hpp"
#include "Ram.hpp"
#include "Dma.hpp"
#include "Timer.hpp"
#include "ShadowAddressSpace.hpp"
#include "Gpu.hpp"
#include "MemoryRegisters.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Thread.hpp>

#include <atomic>

//#define RUN_TESTS
#ifdef RUN_TESTS
#include "ClassicPixelFifo.hpp"
#include "Fetcher.hpp"
#include "Lcdc.hpp"
#endif

class Timer;
class Dma;
class Gpu;
class Speljongen final : public sf::Drawable
{
public:
    Speljongen();
    ~Speljongen();

    Speljongen(const Speljongen&) = delete;
    Speljongen(Speljongen&&) = delete;
    Speljongen& operator = (const Speljongen&) = delete;
    Speljongen& operator = (Speljongen&&) = delete;

    void start();
    void stop();
    void reset();

    bool tick();
    void step();

    void load(const std::string&);

private:
    std::vector<std::uint8_t> m_storage;
    bool m_running;
    Mmu m_mmu;
    SpeedMode m_speedMode;
    MemoryRegisters m_memoryRegisters;
    InterruptManager m_interruptManager;
    Display m_display;
    Cpu m_cpu;

    Ram m_oamRam;
    ShadowAddressSpace m_shadowSpace;
    Timer m_timer;
    Dma m_dma;
    Gpu m_gpu;

    bool m_requestRefresh;
    bool m_lcdDisabled;

    void initRegisters();

    sf::Thread m_thread;
    void threadFunc();

    mutable sf::Text m_text;
    sf::Font m_font;
    void initRenderer();
    void updateDebug();
    void draw(sf::RenderTarget&, sf::RenderStates) const override;

#ifdef RUN_TESTS
    void testTiming();
    void assertTiming(std::int32_t, std::vector<std::uint8_t>);

    class FifoTest final
    {
    public:
        explicit FifoTest(Display&);

        void testEnqueue();
        void testDequeue();
        void testZip();


    private:
        std::vector<std::uint8_t> m_storage;
        Lcdc m_lcdc;
        MemoryRegisters<GpuRegister> m_registers;
        ClassicPixelFifo m_fifo;

        PixelLine zip(std::uint8_t, std::uint8_t, bool);
        void assertEqual(std::uint8_t, std::uint8_t, const std::string&);
    }m_fifoTest;

#endif
};