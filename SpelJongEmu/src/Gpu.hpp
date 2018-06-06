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

#include "AddressSpace.hpp"
#include "Ram.hpp"
#include "GpuPhase.hpp"
#include "OamSearchPhase.hpp"
#include "PixelTransferPhase.hpp"
#include "ColourPalette.hpp"
#include "MemoryRegisters.hpp"
#include "Lcdc.hpp"
#include "Dma.hpp"

class Display;
class InterruptManager;
class SpeedMode;

class Gpu final : public AddressSpace
{
public:
    enum class Mode
    {
        HBlank, VBlank, OamSearch, PixelTransfer, None //order is important!
    };

    Gpu(std::vector<std::uint8_t>&, Display&, InterruptManager&, SpeedMode&);

    void reset();

    Ram& getVRam0();
    Ram& getVRam1();

    std::string getLabel() const override { return "GPU"; }

    bool accepts(std::uint16_t) const override;
    void setByte(std::uint16_t, std::uint8_t) override;
    std::uint8_t getByte(std::uint16_t) const override;
    Mode tick();
    std::uint16_t getTicksInLine() const;
    Lcdc& getLcdc();
    bool isColour() const;
    ColourPalette& getBgPalette();
    Mode getMode() const;
    bool isLcdEnabled() const;

    bool vramUpdated() const;

    void enableColour(bool enable);

private:
    Display & m_display;
    InterruptManager& m_interruptManager;
    Dma m_dma;
    Ram m_oamRam;
    bool m_colour;

    Ram m_videoRam0;
    Ram m_videoRam1;
    Lcdc m_lcdc;
    ColourPalette m_bgPalette;
    ColourPalette m_oamPalette;

    MemoryRegisters m_registers;

    HBlankPhase m_hblankPhase;
    VBlankPhase m_vblankPhase;
    OamSearchPhase m_oamSearchPhase;
    PixelTransferPhase m_transferPhase;
    GpuPhase* m_currentPhase;

    bool m_lcdEnabled;
    std::int16_t m_lcdEnableDelay;
    std::uint16_t m_ticksInLine;
    Mode m_currentMode;

    mutable bool m_vramUpdated;

    void requestLcdInterrupt(std::uint8_t);
    void requestLycEqualsLyInterrupt(); //can you tell this was ported from java?
    std::uint8_t getStat() const;
    void setStat(std::uint8_t);
    void setLcdc(std::uint8_t);
    void disableLCD();
    void enableLCD();
};