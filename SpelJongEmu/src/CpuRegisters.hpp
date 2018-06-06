/*
MIT License

Copyright(c) 2017 Tomasz R?kawek(Coffee GB)
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

#include "BitUtil.hpp"

#include <cstdint>

class Flags final
{
public:
    enum ID
    {
        Z = (1 << 7),
        N = (1 << 6),
        H = (1 << 5),
        C = (1 << 4)
    };

    std::uint8_t getFlags() const { return m_flags; }
    void setFlags(std::uint8_t value) { m_flags = (value & 0xf0); }

    bool isSet(ID id) const { return (m_flags & id) != 0; }

    void set(ID id, bool high) { if (high) { m_flags |= id; } else { m_flags &= ~id; } }

private:
    std::uint8_t m_flags = 0;

};

class Registers final
{
public:

    std::uint8_t getA() const { return a; }
    std::uint8_t getB() const { return b; }
    std::uint8_t getC() const { return c; }
    std::uint8_t getD() const { return d; }
    std::uint8_t getE() const { return e; }
    std::uint8_t getH() const { return h; }
    std::uint8_t getL() const { return l; }

    std::uint16_t getAF() const { return (a << 8) | m_flags.getFlags(); }
    std::uint16_t getBC() const { return (b << 8) | c; }
    std::uint16_t getDE() const { return (d << 8) | e; }
    std::uint16_t getHL() const { return (h << 8) | l; }

    std::uint16_t getSP() const { return sp; }
    std::uint16_t getPC() const { return pc; }

    Flags& getFlags() { return m_flags; }
    const Flags& getFlags() const { return m_flags; }
    
    void setA(std::uint8_t value) { a = value; }
    void setB(std::uint8_t value) { b = value; }
    void setC(std::uint8_t value) { c = value; }
    void setD(std::uint8_t value) { d = value; }
    void setE(std::uint8_t value) { e = value; }
    void setH(std::uint8_t value) { h = value; }
    void setL(std::uint8_t value) { l = value; }

    void setAF(std::uint16_t value)
    {
        a = BitUtil::getMSB(value);
        m_flags.setFlags(BitUtil::getLSB(value));
    }

    void setBC(std::uint16_t value)
    {
        b = BitUtil::getMSB(value);
        c = BitUtil::getLSB(value);
    }

    void setDE(std::uint16_t value)
    {
        d = BitUtil::getMSB(value);
        e = BitUtil::getLSB(value);
    }

    void setHL(std::uint16_t value)
    {
        h = BitUtil::getMSB(value);
        l = BitUtil::getLSB(value);
    }

    void setPC(std::uint16_t value) { pc = value; }
    void setSP(std::uint16_t value) { sp = value; }

    void incrementPC() { pc++; }
    void incrementSP() { sp++; }
    void decrementSP() { sp--; }

private:
    std::uint8_t a = 0;
    std::uint8_t b = 0;
    std::uint8_t c = 0;
    std::uint8_t d = 0;
    std::uint8_t e = 0;
    std::uint8_t h = 0;
    std::uint8_t l = 0;

    std::uint16_t pc = 0x0;
    std::uint16_t sp = 0xfffe;

    Flags m_flags;
};