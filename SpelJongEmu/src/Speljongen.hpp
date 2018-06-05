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
#include "VramViewer.hpp"
#include "Cartridge.hpp"
#include "RamSpace.hpp"
#include "Disassembler.hpp"
#include "ColourRam.hpp"
#include "ColourRegisters.hpp"
#include "Hdma.hpp"

#include <SFML/System/Thread.hpp>

#include <atomic>

//#define RUN_TESTS
#ifdef RUN_TESTS
#include "ClassicPixelFifo.hpp"
#include "Fetcher.hpp"
#include "Lcdc.hpp"
#endif

#define USE_THREADING //launches emulation in own thread if defined

class Timer;
class Dma;
class Gpu;
class Speljongen final
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
#ifndef USE_THREADING
    void update();
#endif 
    void step();

    void load(const std::string&);

    void doImgui() const;
    void lockDisplay();
    void freeDisplay();

private:

    
    std::vector<std::uint8_t> m_storage;
    std::atomic<bool> m_running;
    Mmu m_mmu;
    SpeedMode m_speedMode;
    InterruptManager m_interruptManager;
    Display m_display;
    Cpu m_cpu;
    Timer m_timer;
    ShadowAddressSpace m_shadowSpace;

    //ram is split in two because the upper half is 
    //arranged slightly differently on a GBC
    RamSpace m_lowerRamSpace;
    ColourRam m_colourRam;
    ColourRegisters m_colourRegisters;
    Hdma m_hdma;
    //RamSpace m_upperRamSpace;

    Gpu m_gpu;

    Disassembler m_disassembler;

    Cartridge m_cartridge;
#ifdef USE_THREADING
    sf::Thread m_thread;
    void threadFunc();
#endif
    bool m_requestRefresh;
    bool m_lcdDisabled;

    void initRegisters(bool);

    bool tick();


    VramViewer m_vramViewer;
    void updateDebug();
    void updateVramView();

    mutable sf::Mutex m_mutex;
    std::string m_registerString;
    std::string m_flagsString;
    std::atomic<float> m_tickTime;

    void initMMU(bool colour = false);

#ifdef RUN_TESTS
    void testTiming();
    void assertTiming(std::int32_t, std::vector<std::uint8_t>);

    void testZFlags();
    void assertZFlags(std::vector<std::uint8_t>);

    void testRomFault();

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
        MemoryRegisters m_registers;
        ClassicPixelFifo m_fifo;

        PixelLine zip(std::uint8_t, std::uint8_t, bool);
        void assertEqual(std::uint8_t, std::uint8_t, const std::string&);
    }m_fifoTest;

#endif
};