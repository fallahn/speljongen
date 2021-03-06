#include "Speljongen.hpp"
#include "OpCode.hpp"
#include "CpuRegisters.hpp"

#include <iostream>
#include <iomanip>

#ifdef _WIN32
#include <Windows.h>
#include <VersionHelpers.h>
#endif

#ifdef RUN_TESTS
namespace
{
    std::string redBegin("\x1b[31;1;0m");
    std::string greenBegin("\x1b[32;1;0m");
    std::string colourEnd("\x1b[0m");
}

void Speljongen::testRomFault()
{
    //current fault:
    /*
    FA ld, a (a16) should load the value 0x11 stored at 0xD800
    into A
    E6 AND a, d8 should then AND 0x10 with A and result with 
    Z flag not set (not zero). Currently flag is set...
    */

    m_mmu.setByte(0xD800, 0x11);

    std::vector<std::uint8_t> src =
    {
        //0xFA, 0x00, 0xD8, //ld a, (D800)
        //0xE6, 0x11, //AND a, 0x10
        //0xC4, 0x10, 0xD8 //call NZ D810
        0x7D, //ld a,l
        0x76 //halt
    };

    static const std::uint16_t Offset = 0x100;

    for (auto i = 0u; i <src.size(); ++i)
    {
        m_mmu.setByte(Offset + i, src[i]);
    }
    m_mmu.setByte(0xD810, 0x76); //should jump to this HALT
    m_cpu.clearState();
    m_cpu.getRegisters().setPC(Offset);
    m_cpu.getRegisters().setL(0x3e);

    std::cout << "A is: " << (int)m_cpu.getRegisters().getA() << ", L is: " << (int)m_cpu.getRegisters().getL() << "\n";

    OpCode opcode;
    do
    {
        m_cpu.tick();
        auto old = opcode;
        opcode = m_cpu.getCurrentOpcode();
        if (opcode.getOpcode() != old.getOpcode())
        {
            std::cout << opcode.getLabel() << "\n";  
        }
        std::cout << "A is: " << (int)m_cpu.getRegisters().getA() << ", L is: " << (int)m_cpu.getRegisters().getL() << "\n";

    } while (m_cpu.getCurrentOpcode().getOpcode() != 0x76);
}

void Speljongen::testTiming()
{
    //to test ret we need the stack pointing as if it had a return address on it..
    m_cpu.getRegisters().setSP(0xfffc);
    assertTiming(16, { 0xc9, 0, 0 }); // RET
    m_cpu.getRegisters().setSP(0xfffc);
    assertTiming(16, { 0xd9, 0, 0 }); // RETI
    m_cpu.getRegisters().setSP(0xfffc);
    m_cpu.getRegisters().getFlags().set(Flags::Z, false);
    assertTiming(20, { 0xc0, 0, 0 }); // RET NZ
    m_cpu.getRegisters().setSP(0xfffc);
    m_cpu.getRegisters().getFlags().set(Flags::Z, true);
    m_cpu.getRegisters().setSP(0xfffc);
    assertTiming(8, { 0xc0, 0, 0 }); // RET NZ
    assertTiming(24, { 0xcd, 0, 0 }); // CALL a16
    assertTiming(16, { 0xc5 }); // PUSH BC
    assertTiming(12, { 0xf1 }); // POP AF

    assertTiming(8, { 0xd6, 00 }); // SUB A,d8

    m_cpu.getRegisters().getFlags().set(Flags::C,true);
    assertTiming(8, { 0x30, 00 }); // JR nc,r8

    m_cpu.getRegisters().getFlags().set(Flags::C,false);
    assertTiming(12, { 0x30, 00 }); // JR nc,r8

    m_cpu.getRegisters().getFlags().set(Flags::C, true);
    assertTiming(12, { 0xd2, 00 }); // JP nc,a16

    m_cpu.getRegisters().getFlags().set(Flags::C, false);
    assertTiming(16, { 0xd2, 00 }); // JP nc,a16

    assertTiming(16, { 0xc3, 00, 00 }); // JP a16

    assertTiming(4, { 0xaf }); // XOR a
    assertTiming(12, { 0xe0, 0x05 }); // LD (ff00+05),A
    assertTiming(12, { 0xf0, 0x05 }); // LD A,(ff00+05)
    assertTiming(4, { 0xb7 }); // OR

    assertTiming(4, { 0x7b }); // LDA A,E
    assertTiming(8, { 0xd6, 0x00 }); // SUB A,d8
    assertTiming(8, { 0xcb, 0x12 }); // RL D
    assertTiming(4, { 0x87 }); // ADD A
    assertTiming(4, { 0xf3 }); // DI
    assertTiming(8, { 0x32 }); // LD (HL-),A
    assertTiming(12, { 0x36 }); // LD (HL),d8
    assertTiming(16, { 0xea, 0x00, 0x00 }); // LD (a16),A
    assertTiming(8, { 0x09 }); // ADD HL,BC
    assertTiming(16, { 0xc7 }); // RST 00H


    assertTiming(8, { 0x3e, 0x51 }); // LDA A,51
    assertTiming(4, { 0x1f }); // RRA
    assertTiming(8, { 0xce, 0x01 }); // ADC A,01
    assertTiming(4, { 0x00 }); // NOP
}

void Speljongen::assertTiming(std::int32_t expected, std::vector<std::uint8_t> opcodes)
{
    static const std::uint16_t Offset = 0x100;

    for (auto i = 0u; i < opcodes.size(); ++i)
    {
        m_mmu.setByte(Offset + i, opcodes[i]);
    }
    m_cpu.clearState();
    m_cpu.getRegisters().setPC(Offset);

    std::int32_t ticks = 0;
    OpCode opcode;

    do
    {
        m_cpu.tick();
        if (!opcode && m_cpu.getCurrentOpcode())
        {
            opcode = m_cpu.getCurrentOpcode();
        }
        ticks++;
    }
    while (m_cpu.getState() != Cpu::State::OPCODE || ticks < 4);

    if (!opcode)
    {
        if (expected != ticks)
        {
            std::cout << "Invalid timing for ";
            for (auto o : opcodes) std::cout << std::hex << std::setfill('0') << std::setw(2) << o << ", \n";
        }
        else
        {
            for (auto o : opcodes) std::cout << std::hex << std::setfill('0') << std::setw(2) << o << ", PASSED\n";
        }
    }
    else
    {
        if (expected != ticks)
        {
            std::cout << opcode.getLabel() << ": Invalid timing, Expected: " << expected << ", got " << ticks << "\n";
        }
        else
        {
            std::cout << opcode.getLabel() << ": PASSED\n";
        }
    }
}

void Speljongen::testZFlags()
{
    std::cout << "\n";
    
    //ADD
    m_cpu.getRegisters().setA(128);
    m_cpu.getRegisters().setB(128);
    assertZFlags({ 0x80 });

    m_cpu.getRegisters().setA(128);
    m_cpu.getRegisters().setB(18);
    assertZFlags({ 0x80 });

    //SUB
    m_cpu.getRegisters().setA(120);
    m_cpu.getRegisters().setB(120);
    assertZFlags({ 0x90 });

    m_cpu.getRegisters().setA(160);
    m_cpu.getRegisters().setB(120);
    assertZFlags({ 0x90 });

    //pop af
    m_cpu.getRegisters().setA(120);
    m_cpu.getRegisters().setSP(0xfffc);
    m_mmu.setByte(0xfffc, 0xf0);
    m_mmu.setByte(0xfffd, 0x0);
    assertZFlags({ 0xf1 });

    //AND
    m_cpu.getRegisters().setA(0xf0);
    m_cpu.getRegisters().setB(0x0f);
    assertZFlags({ 0xA0 });

    m_cpu.getRegisters().setA(0xf0);
    m_cpu.getRegisters().setB(0xff);
    assertZFlags({ 0xA0 });

    //OR
    m_cpu.getRegisters().setA(0x01);
    m_cpu.getRegisters().setB(0x0);
    assertZFlags({ 0xB0 });

    m_cpu.getRegisters().setA(0x0);
    m_cpu.getRegisters().setB(0x0);
    assertZFlags({ 0xB0 });

    //INC
    m_cpu.getRegisters().setA(0xff);
    assertZFlags({ 0x3C });

    //DEC
    m_cpu.getRegisters().setA(0x01);
    assertZFlags({ 0x3D });

    //cp
    m_cpu.getRegisters().setA(0x0);
    assertZFlags({ 0xBF });

    //xor
    m_cpu.getRegisters().setA(0x0F);
    assertZFlags({ 0xAF });

    //ADC
    m_cpu.getRegisters().setA(128);
    m_cpu.getRegisters().setB(128);
    assertZFlags({ 0x88 });

    //SBC
    m_cpu.getRegisters().setA(120);
    m_cpu.getRegisters().setB(120);
    assertZFlags({ 0x98 });

    //RLC
    m_cpu.getRegisters().setA(128);
    assertZFlags({ 0xCB, 0x07 });

    //RRC
    assertZFlags({ 0xCB, 0x0f });

    //RL
    m_cpu.getRegisters().setA(128);
    assertZFlags({ 0xCB, 0x17 });

    //RR
    assertZFlags({ 0xCB, 0x1f });

    //SLA
    m_cpu.getRegisters().setA(129);
    assertZFlags({ 0xCB, 0x27 });

    //SRA
    assertZFlags({ 0xCB, 0x2f });

    //TODO SWAP
    //TODO SRL

    //BIT 0
    m_cpu.getRegisters().setA(0);
    assertZFlags({ 0xCB, 0x47 });
}

void Speljongen::assertZFlags(std::vector<std::uint8_t> opcodes)
{
    static const std::uint16_t Offset = 0x100;

    for (auto i = 0u; i < opcodes.size(); ++i)
    {
        m_mmu.setByte(Offset + i, opcodes[i]);
    }
    m_cpu.clearState();
    m_cpu.getRegisters().setPC(Offset);

    OpCode opcode;
    std::uint16_t ticks = 0;
    do
    {
        m_cpu.tick();
        if (!opcode && m_cpu.getCurrentOpcode())
        {
            opcode = m_cpu.getCurrentOpcode();
        }
        ticks++;
    } while (m_cpu.getState() != Cpu::State::OPCODE || ticks < 4);

    if (m_cpu.getRegisters().getFlags().isSet(Flags::Z) == static_cast<bool>(m_cpu.getRegisters().getA()))
    {
        std::cout << opcode.getLabel() << ": FAILED. Z flag is " << m_cpu.getRegisters().getFlags().isSet(Flags::Z) << " and A register is " << (int)m_cpu.getRegisters().getA() << "\n";
    }
    else
    {
        std::cout << opcode.getLabel() << ": PASSED. Z flag is " << m_cpu.getRegisters().getFlags().isSet(Flags::Z) << " and A register is " << (int)m_cpu.getRegisters().getA() << "\n";
    }
}

Speljongen::FifoTest::FifoTest(Display& display)
    : m_storage (0xffff),
    m_lcdc      (m_storage),
    m_registers (m_storage),
    m_fifo      (display, m_lcdc, m_registers)
{
    m_registers.setByte(0xff47, 0b11100100); //default palette

    //if on windows enable virtual terminal output
#ifdef _WIN32
    if (IsWindows10OrGreater())
    {
        // Set output mode to handle virtual terminal sequences
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE)
        {
            std::cout << "Failed to get console handle\n";
        }

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode))
        {
            std::cout << "Failed retrieving console window mode\n";
        }

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(hOut, dwMode))
        {
            std::cout << "Failed enabling Virtual Terminal commands in console\n";
        }
    }
    else
    {
        redBegin = {};
        greenBegin = {};
        colourEnd = {};
    }
#endif
}

void Speljongen::FifoTest::testEnqueue()
{
    m_fifo.enqueue8Pixels(zip(0b11001001, 0b11110000, false), {});
    auto& px = m_fifo.getPixels();
    assertEqual(3, px[0], "Enqueue 01");
    assertEqual(3, px[1], "Enqueue 02");
    assertEqual(2, px[2], "Enqueue 03");
    assertEqual(2, px[3], "Enqueue 04");
    assertEqual(1, px[4], "Enqueue 05");
    assertEqual(0, px[5], "Enqueue 06");
    assertEqual(0, px[6], "Enqueue 07");
    assertEqual(1, px[7], "Enqueue 08");
    std::cout << "\n";

    m_fifo.clear();
}

void Speljongen::FifoTest::testDequeue()
{
    m_fifo.enqueue8Pixels(zip(0b11001001, 0b11110000, false), {});
    m_fifo.enqueue8Pixels(zip(0b10101011, 0b11100111, false), {});
    assertEqual(0b11, m_fifo.dequeuePixel(), "Dequeue 01");
    assertEqual(0b11, m_fifo.dequeuePixel(), "Dequeue 02");
    assertEqual(0b10, m_fifo.dequeuePixel(), "Dequeue 03");
    assertEqual(0b10, m_fifo.dequeuePixel(), "Dequeue 04");
    assertEqual(0b01, m_fifo.dequeuePixel(), "Dequeue 05");
    std::cout << "\n";

    m_fifo.clear();
}

void Speljongen::FifoTest::testZip()
{
    auto pixels = zip(0b11001001, 0b11110000, false);
    assertEqual(3, pixels[0], "Zip 01");
    assertEqual(3, pixels[1], "Zip 02");
    assertEqual(2, pixels[2], "Zip 03");
    assertEqual(2, pixels[3], "Zip 04");
    assertEqual(1, pixels[4], "Zip 05");
    assertEqual(0, pixels[5], "Zip 06");
    assertEqual(0, pixels[6], "Zip 07");
    assertEqual(1, pixels[7], "Zip 08");

    pixels = zip(0b11001001, 0b11110000, true);
    assertEqual(1, pixels[0], "Zip 09");
    assertEqual(0, pixels[1], "Zip 10");
    assertEqual(0, pixels[2], "Zip 11");
    assertEqual(1, pixels[3], "Zip 12");
    assertEqual(2, pixels[4], "Zip 13");
    assertEqual(2, pixels[5], "Zip 14");
    assertEqual(3, pixels[6], "Zip 15");
    assertEqual(3, pixels[7], "Zip 16");
    std::cout << "\n";

    m_fifo.clear();
}

PixelLine Speljongen::FifoTest::zip(std::uint8_t data1, std::uint8_t data2, bool reverse)
{
    PixelLine line;
    return Fetcher::zip(data1, data2, reverse, line);
}

void Speljongen::FifoTest::assertEqual(std::uint8_t a, std::uint8_t b, const std::string& label)
{
    if (a != b)
    {
        std::cout << label << redBegin + ": FAILED" + colourEnd << " Expected " << (int)a << " got " << (int)b << "\n";
    }
    else
    {
        std::cout << label << greenBegin + ": PASSED" + colourEnd << " got expected " << (int)a << "\n";
    }
}

#endif