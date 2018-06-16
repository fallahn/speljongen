#include "Speljongen.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "imgui/imgui_memory_editor.h"
#include "nfd/nfd.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>

namespace
{
    ImVec2 ButtonSize(80.f, 20.f);
    MemoryEditor memEditor;
    std::array<bool, 0x10000> breakPoints = {};

    //------------------------------
    const std::int32_t gameboyCycles = 4194304 / 60;
    const sf::Time frameTime = sf::milliseconds(1000 / 60);

    auto activeCycles = gameboyCycles;
    std::int32_t maxUpdates = 3;
    float frameSkip = 1.f;

    sf::Clock tickClock;
    sf::Clock skipClock;
    sf::Time accumulator;
    std::size_t goodFrames = 0;
    //------------------------------
}

Speljongen::Speljongen()
    : m_storage         (0x10000),
    m_running           (false),
    m_mmu               (m_storage),
    m_speedMode         (m_storage),
    m_interruptManager  (m_storage),
    m_controller        (m_interruptManager, m_mmu),
    m_cpu               (m_mmu, m_interruptManager, m_speedMode, m_display),
    m_timer             (m_storage, m_interruptManager, m_speedMode),
    m_shadowSpace       (m_storage, 0xe000, 0xc000, 0x1e00),
    m_lowerRamSpace     (m_storage, 0xc000, 0x1000, 0x2000),
    m_colourRam         (m_storage),
    m_colourRegisters   (m_storage),
    m_hdma              (m_storage, m_mmu),
    m_gpu               (m_storage, m_display, m_interruptManager, m_speedMode),
    m_cartridge         (m_storage),
#ifdef USE_THREADING
    m_thread            (&Speljongen::threadFunc, this),
#endif
    m_requestRefresh    (false),
    m_lcdDisabled       (false),
    m_vramViewer        (m_mmu),
    m_tickTime          (0.f)
#ifdef RUN_TESTS
    ,m_fifoTest         (m_display)
#endif
{
    initMMU(false);

    m_disassembler.disassemble(m_mmu);
    memEditor.BreakPoints = breakPoints.data();
    tickClock.restart();

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
void Speljongen::handleEvent(const sf::Event& evt)
{
    m_controller.handleEvent(evt);


    if (evt.type == sf::Event::KeyReleased)
    {
        switch (evt.key.code)
        {
        case sf::Keyboard::F9:
            if (!m_running)
            {
                start();
            }
            else
            {
                stop();
            }
            break;
        default: break;
        }
    }
    else if (evt.type == sf::Event::KeyPressed)
    {
        switch (evt.key.code)
        {
        default: break;
        case sf::Keyboard::F3:
            if (!m_running)
            {
                step();
            }
            break;
        }
    }
    else if (evt.type == sf::Event::MouseButtonReleased)
    {
        if (evt.mouseButton.button == sf::Mouse::Right)
        {
            browseFile();
        }
    }
}

void Speljongen::updateDebugger()
{
    if (m_running
        && breakPoints[m_cpu.getRegisters().getPC()])
    {
        stop();
    }
}

void Speljongen::start()
{
#ifndef USE_THREADING
    tickClock.restart();
#else
    m_thread.launch();
#endif
    m_running = true;
}

void Speljongen::stop()
{
    m_running = false;
#ifdef USE_THREADING
    std::cout << "Stopping, please wait...\n";
    m_thread.wait();
#endif
    m_disassembler.disassemble(m_mmu, 0xc000, 0xfe00);
    m_disassembler.disassemble(m_mmu, 0xff80, 0xffff);
    m_disassembler.disassemble(m_gpu.getVRam0(), 0x9800, 0xa000);
    
    memEditor.GotoAddrAndHighlight(m_cpu.getRegisters().getPC(), m_cpu.getRegisters().getPC() +1);
}

void Speljongen::reset()
{  
    m_cpu.reset();
    m_gpu.reset();
    m_mmu.reset();
    m_display.clear();

    activeCycles = gameboyCycles;
    frameSkip = 1.f;
    m_cpu.getRegisters().setPC(0x100);

    updateDebug();
}

#ifndef USE_THREADING
void Speljongen::update()
{
    auto duration = tickClock.restart();
    accumulator += duration;
    maxUpdates = 8;
    while (accumulator > frameTime && maxUpdates--)
    {
        skipClock.restart();

        std::int32_t tickCounter = 0;
        while (tickCounter++ < activeCycles && m_running)
        {
            tick();
        }

        auto skipTime = skipClock.getElapsedTime();
        m_tickTime = (100.f * (frameTime.asSeconds() / std::max(frameTime.asSeconds(), skipTime.asSeconds()))) * frameSkip;

        accumulator -= frameTime;
        updateVramView();
        updateDebug();

        if (maxUpdates == 0)
        {
            std::cout << "Warning slow emulation - enabling frame skip\n";
            activeCycles /= 2;
            frameSkip /= 2.f;
            accumulator = sf::Time();
            goodFrames = 0;
        }
        else if (maxUpdates == 7 && goodFrames < 10)
        {
            goodFrames++;
            if (goodFrames == 10)
            {
                std::cout << "Reducing frame skip...\n";
                activeCycles = std::min(gameboyCycles, activeCycles * 2);
                frameSkip = std::min(1.f, frameSkip * 2.f);
                accumulator = sf::Time();
            }
        }

        updateDebugger();
    }
}
#endif

void Speljongen::step()
{
    while (!tick()) {}
    updateDebug();
    updateVramView();

    auto pc = m_cpu.getRegisters().getPC();
    memEditor.GotoAddrAndHighlight(pc, pc + 1);

    m_disassembler.disassemble(m_mmu, (pc > 5) ? pc - 5 : 0, (pc < 0xffff - 5) ? pc + 5 : 0xffff); //slow as all balls in debug mode
}

void Speljongen::load(const std::string& path)
{
    reset();    
    
    if (m_cartridge.load(path))
    {
        std::cout << "Loaded " << m_cartridge.getTitle() << "!\n";

        m_mmu.insertCartridge(m_cartridge);
        initMMU(m_cartridge.isColour());
        m_cpu.getRegisters().setPC(0x100);

        m_disassembler.disassemble(m_mmu, 0, 0x7fff);
        //m_disassembler.updateRawView(m_mmu);
        memEditor.GotoAddrAndHighlight(0x100, 0x101);
    }
}

void Speljongen::doImgui()
{
    auto flags = m_cpu.getRegisters().getFlags();
    bool flagsZ = flags.isSet(Flags::Z);
    bool flagsC = flags.isSet(Flags::C);
    bool flagsH = flags.isSet(Flags::H);
    bool flagsN = flags.isSet(Flags::N);
    
    ImGui::SetNextWindowSize({ 434.f, 340.f });
    ImGui::SetNextWindowPos({ 356.f, 10.f });
    ImGui::Begin("Gpu and register status", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.129f, 0.192f, 0.349f, 1.f });
    
    ImGui::BeginChild("Video", ImVec2(276.f, 280.f), false);
    if (ImGui::CollapsingHeader("VRAM"))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 1.f, 0.f });
        ImGui::Image(m_vramViewer.getTexture(), sf::Vector2f(256.f, 384.f));
        ImGui::PopStyleVar();
    }
    if (ImGui::CollapsingHeader("OAM Data", ImGuiTreeNodeFlags_DefaultOpen))
    {
        std::uint16_t address = 0xfe00;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f , 0.f });
        ImGui::Columns(4);
        for (auto i = 0; i < 40; ++i)
        {
            if(ImGui::GetColumnIndex() == 0) ImGui::Separator();
            ImGui::Text("%.2x", m_mmu.getByte(address++));
            ImGui::SameLine();
            ImGui::Text("%.2x", m_mmu.getByte(address++));
            auto tileID = m_mmu.getByte(address++);
            ImGui::Text("%.2x", tileID);
            ImGui::SameLine();
            auto flags = m_mmu.getByte(address++);
            ImGui::Text("%.2x", flags);

            auto x = tileID % 16;
            auto y = tileID / 16;
            sf::FloatRect rect(8.f * x, 8.f * y, 8.f, 8.f);
            if (flags & (1 << 6))//y flip
            {
                rect.top += 8.f;
                rect.height = -rect.height;
            }
            if (flags & (1 << 5)) //x flip
            {
                rect.left += 8.f;
                rect.width = -rect.width;
            }
            ImGui::SameLine();
            ImGui::Image(m_vramViewer.getTexture(), rect);

            ImGui::NextColumn();
        }
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::BeginChild("Registers", ImVec2(154.f, 280.f), false, ImGuiWindowFlags_HorizontalScrollbar);
#ifdef USE_THREADING
    m_mutex.lock();
#endif
    ImGui::Text("%s", m_registerString.c_str());
#ifdef USE_THREADING
    m_mutex.unlock();
#endif // USE_THREADING
    //ImGui::Text("%s", m_cartridge.getInfo().c_str());
    ImGui::EndChild();

    ImGui::Checkbox("Z", &flagsZ);
    ImGui::SameLine();
    ImGui::Checkbox("C", &flagsC);
    ImGui::SameLine();
    ImGui::Checkbox("H", &flagsH);
    ImGui::SameLine();
    ImGui::Checkbox("N", &flagsN);
    ImGui::SameLine();
    ImGui::Text("%s", m_running ? "        Running" : "        Stopped");
    ImGui::End();

    std::string title = "Gameboy - " + m_cartridge.getTitle();
    ImGui::SetNextWindowSize({ 336.f, 340.f });
    ImGui::SetNextWindowPos({ 10.f, 10.f });
    ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::Image(m_display.getTexture(), sf::Vector2f(320.f, 288.f));
    ImGui::Text("Display FPS: %.2f", ImGui::GetIO().Framerate);
    ImGui::SameLine();
    ImGui::Text("  Emulation speed: %.2f%%", m_tickTime.load());
    ImGui::End();

    ImGui::SetNextWindowSize({ 780.f, 230.f });
    ImGui::SetNextWindowPos({ 10.f, 360.f });
    ImGui::Begin("DASM - F3 Step, F9 Run/Stop, Right click to load ROM", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::BeginChild("Output", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 194.f), true, ImGuiWindowFlags_HorizontalScrollbar);
    memEditor.DrawContents(m_disassembler.getRawView().data(), m_disassembler.getRawView().size(), 0, &m_disassembler.getLabels());
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("Control", ImVec2((ImGui::GetWindowContentRegionWidth() * 0.5f) - 8.f, 194.f), true/*, ImGuiWindowFlags_HorizontalScrollbar*/);
    if (ImGui::Button("Load", ButtonSize) && !m_running)
    {
        browseFile();
    }
    ImGui::SameLine();
    if (ImGui::Button(m_running ? "Stop (F9)" : "Run (F9)", ButtonSize))
    {
        m_running ? stop() : start();
    }
    ImGui::SameLine();
    if (ImGui::Button("Step (F3)", ButtonSize) && !m_running)
    {
        step();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset", ButtonSize) && !m_running)
    {
        reset();
    }
    ImGui::Text("%s", m_cartridge.getInfo().c_str());
    ImGui::EndChild();
    ImGui::End();

    //ImGui::ShowDemoWindow();
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
bool Speljongen::tick()
{
#ifdef RUN_TESTS
    return true;
#endif

    m_timer.tick();
    m_controller.tick();
    auto ret = false;
    
    if (m_hdma.transferInProgress())
    {
        m_hdma.tick();
    }
    else
    {
        ret = m_cpu.tick();
    }
    auto gpuMode = m_gpu.tick();
    if (gpuMode != Gpu::Mode::None)
    {
        //update hdma
        m_hdma.gpuUpdate(gpuMode);
    }

    if (!m_lcdDisabled && !m_gpu.isLcdEnabled())
    {
        m_lcdDisabled = true;
        m_display.refresh();
        m_hdma.onLcdSwitch(false);
    }
    else if (gpuMode == Gpu::Mode::VBlank)
    {
        m_requestRefresh = true;
        m_display.refresh();
    }

    if (m_lcdDisabled && m_gpu.isLcdEnabled())
    {
        m_lcdDisabled = false;
        m_hdma.onLcdSwitch(true);
    }
    else if (m_requestRefresh && gpuMode == Gpu::Mode::OamSearch)
    {
        m_requestRefresh = false;
    }

    return ret;
}

#ifdef USE_THREADING
void Speljongen::threadFunc()
{
    tickClock.restart();

    while (m_running)
    {
        auto duration = tickClock.restart();
        accumulator += duration;
        maxUpdates = 8;
        while(accumulator > frameTime && maxUpdates--)
        {
            std::int32_t tickCounter = 0;
            while (tickCounter++ < activeCycles && m_running)
            {
                tick();
            }

            m_tickTime = (100.f / (accumulator.asSeconds() / frameTime.asSeconds())) * frameSkip;

            accumulator -= frameTime;
            updateVramView();
            updateDebug();

            if (maxUpdates == 0)
            {
                std::cout << "Warning slow emulation - enabling frame skip\n";
                activeCycles /= 2;
                frameSkip /= 2.f;
                accumulator = sf::Time();
            }
        }
    }
    std::cout << "Stopped.\n";
}
#endif

void Speljongen::initRegisters(bool colour)
{
    Registers& r = m_cpu.getRegisters();

    r.setAF(0x01b0);
    if (colour) 
    {
        r.setA(0x11);
    }
    r.setBC(0x0013);
    r.setDE(0x00d8);
    r.setHL(0x014d);
    r.setSP(0xfffe);
    r.setPC(0x0000);

    m_mmu.setByte(MemoryRegisters::IF, 0xe1);
    m_mmu.setByte(0xfffe, 0);
    m_interruptManager.disableInterrupts(false);
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
    ss << " : 0x" << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(reg.getSP());
    ss << "\nPC: 0x" << std::setfill('0') << std::setw(4) << reg.getPC();
    ss << " : 0x" << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(reg.getPC());
    //ss << "\n" << (int)m_cpu.getCurrentOpcode().getOpcode() << ", " << m_cpu.getCurrentOpcode().getLabel();

    ss << "\n\nLCDC: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(MemoryRegisters::LCDC);
    ss << "\nSTAT: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(0xff41);
    ss << "\nLY: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(0xff44); 
    ss << "\nIE: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(0xffff);
    ss << "\nIF: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(MemoryRegisters::IF);

    ss << "\nBGP: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(MemoryRegisters::BGP);
    ss << "\nOBP0: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(MemoryRegisters::OBP0);
    ss << "\nOBP1: " << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(MemoryRegisters::OBP1);

#ifdef USE_THREADING
    m_mutex.lock();
#endif
    m_registerString = ss.str();
#ifdef USE_THREADING
    m_mutex.unlock();
#endif
}

void Speljongen::updateVramView()
{
    //only want to do this if VRAM flagged as changed, causes significant slow down
    if (!m_gpu.vramUpdated()) return;
    
    std::uint16_t address = 0x8000;

    for (auto gridY = 0; gridY < 24; ++gridY)
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

                auto byte0 = m_gpu.getVRam0().getByte(address++);
                auto byte1 = m_gpu.getVRam0().getByte(address++);

                for (auto i = 7; i >= 0; --i)
                {
                    std::uint8_t colour = (byte0 & (1 << i)) ? 1 : 0;
                    colour |= (byte1 & (1 << i)) ? 2 : 0;

                    m_vramViewer.setPixel(x + (7 - i), y, colour);
                }
            }
        }
    }
    //std::cout << std::hex << address << "\n";

    /*std::cout << "\n\n";
    for (auto i = address; i < address + 0x400; ++i)
    {
        if (i % 16 == 0)
        {
            std::cout << "\n" << i << " ";
        }

        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)m_mmu.getByte(i) << " ";
    }*/

    m_vramViewer.update();
}

void Speljongen::initMMU(bool colour)
{
    //TODO disable colour on sound
    m_interruptManager.enableColour(colour);
    m_gpu.enableColour(colour);

    //maps address spaces so they are accessable through mmu
    /*
    This is a bit kludgy and makes the order in which the spaces
    are added important. For example the interrupt manager must
    be added after the GPU to make sure the correct accessor is
    used when reading or writing the MMU
    */
    m_mmu.addAddressSpace(m_gpu); //add this last to make sure any colour palette registers are added
    m_mmu.addAddressSpace(m_interruptManager);
    m_mmu.addAddressSpace(m_timer);   
    m_mmu.addAddressSpace(m_shadowSpace);
    m_mmu.addAddressSpace(m_lowerRamSpace);

    if(colour)
    {
        //add speed mode to mmu
        m_mmu.addAddressSpace(m_speedMode);
        //add hdma
        m_mmu.addAddressSpace(m_hdma);
        //add GBC ram
        m_mmu.addAddressSpace(m_colourRam);
        m_mmu.addAddressSpace(m_colourRegisters);
        m_colourRegisters.reset();
    }

    m_mmu.initBios(); //MUST be done after mapping is complete

    //reset all registers
    m_mmu.setByte(MemoryRegisters::TIMA, 0);
    m_mmu.setByte(MemoryRegisters::TMA, 0);
    m_mmu.setByte(MemoryRegisters::TAC, 0);
    //TODO audio registers
    m_mmu.setByte(MemoryRegisters::LCDC, 0x91);
    m_mmu.setByte(MemoryRegisters::SCY, 0);
    m_mmu.setByte(MemoryRegisters::SCX, 0);
    m_mmu.setByte(MemoryRegisters::LYC, 0);
    m_mmu.setByte(MemoryRegisters::LY, 0);
    m_mmu.setByte(MemoryRegisters::BGP, 0xfc);
    m_mmu.setByte(MemoryRegisters::OBP0, 0xff);
    m_mmu.setByte(MemoryRegisters::OBP1, 0xff);
    m_mmu.setByte(MemoryRegisters::WY, 0);
    m_mmu.setByte(MemoryRegisters::WX, 0);
    m_mmu.setByte(0xffff, 0);
        
    initRegisters(colour);
    updateDebug();
}

void Speljongen::browseFile()
{
    stop();

    //TODO check also inside window
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog("gb,gbc,zip", nullptr, &outPath);
    if (result == NFD_OKAY)
    {
        load(outPath);
        free(outPath);
    }
}