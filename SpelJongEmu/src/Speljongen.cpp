#include "Speljongen.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>

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
    m_lowerRamSpace     (m_storage, 0xc000, 0x1000, 0x2000),
    m_upperRamSpace     (m_storage, 0xd000, 0x1000, 0x2000),
    m_dma               (m_storage, m_speedMode),
    m_gpu               (m_storage, m_display, m_interruptManager, m_dma, m_oamRam, m_memoryRegisters, false),
    m_cartridge         (m_storage),
    m_thread            (&Speljongen::threadFunc, this),
    m_requestRefresh    (false),
    m_lcdDisabled       (false)
#ifdef RUN_TESTS
    ,m_fifoTest         (m_display)
#endif
{
    initMMU(false);

#ifdef RUN_TESTS   
    /*m_fifoTest.testEnqueue();
    m_fifoTest.testDequeue();
    m_fifoTest.testZip();
    testTiming();
    testZFlags();*/
    testRomFault();
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
    std::cout << "Stopping, please wait...\n";
    m_thread.wait();
}

void Speljongen::reset()
{
    m_cpu.clearState();
    m_gpu.reset();
    m_display.clear();

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
        m_display.refresh();
        //hdma
    }
    else if (gpuMode == Gpu::Mode::VBlank)
    {        
        m_requestRefresh = true;
        m_display.refresh();
        //updateDebug();
    }

    if (m_lcdDisabled && m_gpu.isLcdEnabled())
    {
        m_lcdDisabled = false;
        //TODO hdma
    }
    else if (m_requestRefresh && gpuMode == Gpu::Mode::OamSearch)
    {
        m_requestRefresh = false;
    }

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
    
    m_mmu.removeCartridge();
    m_cartridge.load(path);
    std::cout << "Loaded " << m_cartridge.getTitle() << "!\n";
    m_mmu.insertCartridge(m_cartridge);
    initMMU(/*m_cartridge.isColour()*/false);
    m_cpu.getRegisters().setPC(0x100);  
}

void Speljongen::doImgui() const
{
    auto flags = m_cpu.getRegisters().getFlags();
    bool flagsZ = flags.isSet(Flags::Z);
    bool flagsC = flags.isSet(Flags::C);
    bool flagsH = flags.isSet(Flags::H);
    bool flagsN = flags.isSet(Flags::N);
    
    ImGui::SetNextWindowSize({ 434.f, 340.f });
    ImGui::SetNextWindowPos({ 356.f, 10.f });
    ImGui::Begin("VRAM and register status", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::Image(m_vramViewer.getTexture(), sf::Vector2f(256.f, 256.f));
    ImGui::SameLine();
    m_mutex.lock();
    ImGui::Text("%s", m_registerString.c_str());
    m_mutex.unlock();
    ImGui::Checkbox("Z", &flagsZ);
    ImGui::SameLine();
    ImGui::Checkbox("C", &flagsC);
    ImGui::SameLine();
    ImGui::Checkbox("H", &flagsH);
    ImGui::SameLine();
    ImGui::Checkbox("N", &flagsN);
    ImGui::End();

    std::string title = "Gameboy - " + m_cartridge.getTitle();
    ImGui::SetNextWindowSize({ 336.f, 340.f });
    ImGui::SetNextWindowPos({ 10.f, 10.f });
    ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::Image(m_display.getTexture(), sf::Vector2f(320.f, 288.f));
    ImGui::Text("Display FPS: %.2f", ImGui::GetIO().Framerate);
    ImGui::SameLine();
    ImGui::Text("Emulation speed: %.2f%%", m_tickTime.load());
    ImGui::End();

    ImGui::SetNextWindowSize({ 780.f, 230.f });
    ImGui::SetNextWindowPos({ 10.f, 360.f });
    ImGui::Begin("Inspector - F3 Step, F9 Run/Stop, Right click to load ROM", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::End();
}

void Speljongen::lockDisplay()
{
    m_display.lockDisplay();
    m_vramViewer.lockDisplay();
}

void Speljongen::freeDisplay()
{
    m_display.freeDisplay();
    m_vramViewer.freeDisplay();
}

//private
void Speljongen::threadFunc()
{
    static const std::int32_t gameboyCycles = 4194304 / 60;
    static const sf::Time frameTime = sf::milliseconds(1000 / 60);

    sf::Clock tickClock;
    sf::Time accumulator;

    while (m_running)
    {
        auto duration = tickClock.restart();
        accumulator += duration;
        while(accumulator > frameTime)
        {
            std::int32_t tickCounter = 0;
            while (tickCounter++ < gameboyCycles && m_running)
            {
                tick();
            }

            m_tickTime = 100.f / (accumulator.asSeconds() / frameTime.asSeconds());

            accumulator -= frameTime;
            updateVramView(); //TODO only want to do this if VRAM flagged as changed, causes significant slow down
            updateDebug();
        }
    }
    std::cout << "Stopped.\n";
}

void Speljongen::initRegisters(bool colour)
{
    Registers& r = m_cpu.getRegisters();

    r.setAF(0x01b0);
    if (colour) {
        r.setA(0x11);
    }
    r.setBC(0x0013);
    r.setDE(0x00d8);
    r.setHL(0x014d);
    r.setSP(0xfffe);
    r.setPC(0x0000);
}

void Speljongen::updateDebug()
{
    auto reg = m_cpu.getRegisters();
    std::stringstream ss;
    ss << std::hex;
    ss << "AF: 0x" << std::setfill('0') << std::setw(4) << reg.getAF();
    ss << "\nBC: 0x" << std::setfill('0') << std::setw(4) << reg.getBC();
    ss << "\nDE: 0x" << std::setfill('0') << std::setw(4) << reg.getDE();
    ss << "\nHL: 0x" << std::setfill('0') << std::setw(4) << reg.getHL();
    ss << "\nSP: 0x" << std::setfill('0') << std::setw(4) << reg.getSP();
    ss << "\nPC: 0x" << std::setfill('0') << std::setw(4) << reg.getPC();
    ss << " : 0x" << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(reg.getPC());
    ss << "\n" << (int)m_cpu.getCurrentOpcode().getOpcode() << ", " << m_cpu.getCurrentOpcode().getLabel();

    ss << "\n\nLCDC: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(MemoryRegisters::LCDC);
    ss << "\nSTAT: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(0xff41);
    ss << "\nLY: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(0xff44); 
    ss << "\nIE: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(0xffff);
    ss << "\nIF: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(MemoryRegisters::IF);

    m_mutex.lock();
    m_registerString = ss.str();
    m_mutex.unlock();
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

void Speljongen::initMMU(bool colour)
{

    //maps address spaces so they are accessable through mmu
    /*
    This is a bit kludgy and makes the order in which the spaces
    are added important. For example the interrupt manager must
    be added after the GPU to make sure the correct accessor is
    used when reading or writing the MMU
    */
    m_mmu.addAddressSpace(m_oamRam);
    m_mmu.addAddressSpace(m_dma);
    m_mmu.addAddressSpace(m_timer);
    m_mmu.addAddressSpace(m_gpu);
    m_mmu.addAddressSpace(m_interruptManager);
    m_mmu.addAddressSpace(m_shadowSpace);
    //m_mmu.addAddressSpace(m_memoryRegisters); //mmu maps these via sub spaces, ie GPU
    m_mmu.addAddressSpace(m_lowerRamSpace);

    if (!colour)
    {
        m_mmu.addAddressSpace(m_upperRamSpace);

        m_mmu.initBios(); //MUST be done after mapping is complete
    }


    m_interruptManager.disableInterrupts(false);
    initRegisters(colour);
    updateDebug();
}