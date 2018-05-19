#include "Speljongen.hpp"
#include "Ram.hpp"
#include "Dma.hpp"
#include "Timer.hpp"
#include "ShadowAddressSpace.hpp"

#include <sstream>
#include <iomanip>

#include <SFML/Graphics/RenderTarget.hpp>

Speljongen::Speljongen()
    : m_speedMode(),
    m_mmu(),
    m_cpu(m_mmu, m_mmu.addAddressSpace<InterruptManager>(false), m_speedMode),
    m_timer(nullptr),
    m_dma(nullptr)
{
    auto& oamRam = m_mmu.addAddressSpace<Ram>(0xfe00, 0x00a0);
    m_dma = &m_mmu.addAddressSpace<Dma>(oamRam, m_speedMode);
    auto& interruptManager = m_mmu.getAddressSpace<InterruptManager>(AddressSpace::Interrupt);
    m_timer = &m_mmu.addAddressSpace<Timer>(interruptManager, m_speedMode);

    //TODO this is classic GB ram - colour GB is split
    //into two blocks with the second half having a specific layout
    m_mmu.addAddressSpace<Ram>(0xc000, 0x2000);
    //else cgb
    /*
    m_mmu.addAddressSpace<Ram>(0xc000, 0x1000);
    //other ram and hdma
    */

    m_mmu.addAddressSpace<Ram>(0xff80, 0x7f);
    m_mmu.addAddressSpace<ShadowAddressSpace>(0xe000, 0xc000, 0x1e00);

    //initRegisters(); //TODO only do this when not using boot rom
    initRenderer();
}

//public
void Speljongen::start()
{
    m_running = true;
    while (m_running)
    {
        tick();
    }
}

void Speljongen::stop()
{
    m_running = false;
}

void Speljongen::tick()
{
    //TODO these can be ticked internally by mmu
    m_timer->tick();
    m_dma->tick();
    m_cpu.tick();

    updateDebug();
}

//private
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
    r.setPC(0x0100);
}

void Speljongen::initRenderer()
{
    m_font.loadFromFile("assets/VeraMono.ttf");
    m_text.setFont(m_font);
    m_text.setCharacterSize(12);
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
    ss << "\nHL: 0x" << std::setfill('0') << std::setw(4) << reg.getHL();
    ss << "\nA: 0x" << std::setfill('0') << std::setw(2) << (int)reg.getA();
    ss << "\nZ: " << reg.getFlags().isSet(Flags::Z) << " C: " << reg.getFlags().isSet(Flags::C);

    m_text.setString(ss.str());
}

void Speljongen::draw(sf::RenderTarget& rt, sf::RenderStates) const
{
    rt.draw(m_text);
}