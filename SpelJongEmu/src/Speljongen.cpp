#include "Speljongen.hpp"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>

#include <SFML/Graphics/RenderTarget.hpp>

Speljongen::Speljongen()
    : m_storage         (0x10000),
    m_mmu               (m_storage),
    m_speedMode         (m_storage),
    m_memoryRegisters   (m_storage),
    m_interruptManager  (m_storage, false),
    m_cpu               (m_mmu, m_interruptManager, m_speedMode, m_display),
    m_timer             (m_storage, m_interruptManager, m_speedMode),
    m_oamRam            (m_storage, 0xfe00, 0x00a0),
    m_shadowSpace       (m_storage, 0xe000, 0xc000, 0x1e00),
    m_dma               (m_storage, m_speedMode),
    m_gpu               (m_storage, m_display, m_interruptManager, m_dma, m_oamRam, m_memoryRegisters, false),
    m_thread            (&Speljongen::threadFunc, this),
    m_requestRefresh    (false),
    m_lcdDisabled       (false)
#ifdef RUN_TESTS
    ,m_fifoTest         (m_display)
#endif
{

    //maps address spaces so they are accessable through mmu
    m_mmu.addAddressSpace(m_oamRam);
    m_mmu.addAddressSpace(m_dma);
    m_mmu.addAddressSpace(m_interruptManager);
    m_mmu.addAddressSpace(m_timer);
    m_mmu.addAddressSpace(m_gpu);
    m_mmu.addAddressSpace(m_shadowSpace);
    //m_mmu.addAddressSpace(m_memoryRegisters); //mmu maps these via sub spaces, ie GPU

    //TODO this is classic GB ram - colour GB is split
    //into two blocks with the second half having a specific layout
    //m_mmu.addAddressSpace<Ram>(0xc000, 0x2000);
    //else cgb
    /*
    m_mmu.addAddressSpace<Ram>(0xc000, 0x1000);
    //other ram and hdma
    */

    //m_mmu.addAddressSpace<Ram>(0xff80, 0x7f);

    m_mmu.initBios(); //MUST be done after mapping is complete

    m_interruptManager.disableInterrupts(false);
    initRegisters(); //TODO only do this when not using boot rom
    initRenderer();

#ifdef RUN_TESTS   
    m_fifoTest.testEnqueue();
    m_fifoTest.testDequeue();
    m_fifoTest.testZip();
    testTiming();
    testZFlags();
#endif
}

Speljongen::~Speljongen()
{
    if (m_running)
    {
        stop();
    }
}

//public
void Speljongen::start()
{
    m_running = true;
    m_thread.launch();
}

void Speljongen::stop()
{
    m_running = false;
    m_thread.wait();
}

void Speljongen::reset()
{
    m_cpu.clearState();

    initRegisters();
    m_cpu.getRegisters().setPC(0x100);

    m_gpu.reset();
    m_mmu.setByte(MemoryRegisters::BGP, 0xfc);

    //clear VRAM
    for (std::uint16_t i = 0x8000; i < 0x9800; ++i)
    {
        m_mmu.setByte(i, 0);
    }

   std::array<char, 6> test = { 'r', 'e', 's', 'e', 't', '\n' };
   for (auto c : test)
   {
       m_mmu.setByte(MemoryRegisters::SB, c);
       m_mmu.setByte(MemoryRegisters::SC, 0x81);
   }

    updateDebug();
}

bool Speljongen::tick()
{
#ifdef RUN_TESTS
    return true;
#endif
    
    m_timer.tick();
    m_dma.tick();
    auto ret = m_cpu.tick();

    auto gpuMode = m_gpu.tick();
    if (!m_lcdDisabled && !m_gpu.isLcdEnabled())
    {
        m_lcdDisabled = true;
        //m_requestRefresh = true;
        m_display.requestRefresh();
        //m_requestRefresh = false;
        //hdma
    }
    else if (gpuMode == Gpu::Mode::VBlank)
    {        
        m_requestRefresh = true;
        m_display.requestRefresh();
        updateDebug();
        //m_requestRefresh = false;
    }

    if (m_lcdDisabled && m_gpu.isLcdEnabled())
    {
        m_lcdDisabled = false;
        //m_display.waitForRefresh();
        //TODO hdma
    }
    else if (m_requestRefresh && gpuMode == Gpu::Mode::OamSearch)
    {
        m_requestRefresh = false;
        //m_display.waitForRefresh();
    }

    //if (m_cpu.getRegisters().getFlags().isSet(Flags::Z))
    //{
    //    std::cout << std::hex << m_cpu.getRegisters().getPC() << "\n";
    //}

    return ret;
}

void Speljongen::step()
{
    while (!tick()) {}
    updateDebug();
}

void Speljongen::load(const std::string& path)
{
    reset();    
    
    std::cout << "loading: " << path << "\n";
    //TODO some file size checking
    std::ifstream file(path, std::ios::binary);

    file.seekg(0, file.end);
    size_t length = file.tellg();
    file.seekg(0, file.beg);
    std::vector<char> buf(length);
    file.read(buf.data(), length);
    file.close();

    std::uint16_t address = 0;
    for (auto c : buf) m_mmu.setByte(address++, c);
}

//private
void Speljongen::threadFunc()
{
    while (m_running)
    {
        tick();
    }
}

void Speljongen::initRegisters()
{
    Registers& r = m_cpu.getRegisters();

    r.setAF(0x01b0);
    /*if (colour) {
        r.setA(0x11);
    }*/
    r.setBC(0x0013);
    r.setDE(0x00d8);
    r.setHL(0x014d);
    r.setSP(0xfffe);
    //r.setPC(0x0100);
    //r.setPC(0);
}

void Speljongen::initRenderer()
{
    m_font.loadFromFile("assets/VeraMono.ttf");
    m_text.setFont(m_font);
    m_text.setCharacterSize(12);
    m_text.setString("LCtrl toggle run/step\nSpace step\n");

    m_display.setPosition(140, 100);
    m_display.setScale(2.f, 2.f);

    m_vramViewer.setPosition(800.f - 312.f, 100.f);
    m_vramViewer.setScale(2.f, 2.f);

    updateDebug();
}

void Speljongen::updateDebug()
{
    const auto& reg = m_cpu.getRegisters();
    std::stringstream ss;
    ss << "LCtrl toggle run/step\nSpace step\n";
    ss << std::hex;
    ss << "PC: 0x" << std::setfill('0') << std::setw(4) << reg.getPC();
    ss << " : 0x" << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(reg.getPC());
    ss << "\nCurrent op : " << (int)m_cpu.getCurrentOpcode().getOpcode() << ", " << m_cpu.getCurrentOpcode().getLabel();
    ss << "\nSP: 0x" << std::setfill('0') << std::setw(4) << reg.getSP();
    ss << "\nBC: 0x" << std::setfill('0') << std::setw(4) << reg.getBC();
    ss << "\nDE: 0x" << std::setfill('0') << std::setw(4) << reg.getDE();
    ss << "\nHL: 0x" << std::setfill('0') << std::setw(4) << reg.getHL();
    ss << "\nA: 0x" << std::setfill('0') << std::setw(2) << (int)reg.getA();
    ss << "\nZ: " << reg.getFlags().isSet(Flags::Z) << " C: " << reg.getFlags().isSet(Flags::C);

    ss << "\n\nLY: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(0xff44);
    ss << "\nSTAT: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(0xff41);
    ss << "\nBGP: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(0xff47);
    ss << "\nSerial: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(0xff01);

    m_text.setString(ss.str());

    updateVramView();
}

void Speljongen::updateVramView()
{
    std::uint16_t address = 0x8000;

    for (auto gridY = 0; gridY < 16; ++gridY)
    {
        for (auto gridX = 0; gridX < 16; ++gridX)
        {
            for (auto pxY = 0; pxY < 8; ++pxY)
            {
                auto x = (gridX * 8);
                auto y = (gridY * 8) + pxY;   

                //OK, so gameboy pixel values are incredibly convoluted:
                /*
                A colour is represented in 2 bits, 0 - 2 decimal. (Which are used as a palette index)
                A row in the tile is represented by 2 consecutive VRAM bytes.
                Each bit in the byte represents 1 pixel index, bit 7 is px0, bit 6 is px1 etc

                The first byte represents the lower bit of a colour value. The second byte contains the upper bits!
                eg: byte 1 0b00111100, byte 2 0b01111110
                which translates to:
                    0b 0 0 1 1 1 1 0 0
                    0b 0 1 1 1 1 1 1 0

                aka:
                    0b00 0b10 0b11 0b11 0b11 0b11 0b10 0b00
                    0,   2,   3,   3,   3,   3,   2,   0
                */

                auto byte0 = m_mmu.getByte(address++);
                auto byte1 = m_mmu.getByte(address++);

                for (auto i = 7; i >= 0; --i)
                {
                    std::uint8_t colour = (byte0 & (1 << i)) ? 1 : 0;
                    colour |= (byte1 & (1 << i)) ? 2 : 0;

                    m_vramViewer.setPixel(x + (7 - i), y, colour);
                }
            }
        }
    }

    m_vramViewer.update();
}

void Speljongen::draw(sf::RenderTarget& rt, sf::RenderStates) const
{
    rt.draw(m_display);
    rt.draw(m_vramViewer);
    rt.draw(m_text);
}