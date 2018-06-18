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
#include "Controller.hpp"
#include "Apu.hpp"

#include <SFML/System/Thread.hpp>

#include <atomic>

//#define RUN_TESTS
#ifdef RUN_TESTS
#include "ClassicPixelFifo.hpp"
#include "Fetcher.hpp"
#include "Lcdc.hpp"
#endif

//#define USE_THREADING //launches emulation in own thread if defined

namespace sf
{
    class Event;
}

class Speljongen final
{
public:
    Speljongen();
    ~Speljongen();

    Speljongen(const Speljongen&) = delete;
    Speljongen(Speljongen&&) = delete;
    Speljongen& operator = (const Speljongen&) = delete;
    Speljongen& operator = (Speljongen&&) = delete;

    void handleEvent(const sf::Event&);
    void updateDebugger();

    void start();
    void stop();
    void reset();
#ifndef USE_THREADING
    void update();
#endif 
    void step();

    void load(const std::string&);

    void doImgui();
    void lockDisplay();
    void freeDisplay();

private:
    std::vector<std::uint8_t> m_storage;
    std::atomic<bool> m_running;
    Mmu m_mmu;
    Apu m_apu;
    SpeedMode m_speedMode;
    InterruptManager m_interruptManager;
    Controller m_controller;
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

#ifdef USE_THREADING
    mutable sf::Mutex m_mutex;
#endif
    std::string m_registerString;
    std::string m_flagsString;
    std::atomic<float> m_tickTime;

    void initMMU(bool colour = false);

    void browseFile();

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