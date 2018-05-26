#pragma once

#include "AddressSpace.hpp"
#include "Ram.hpp"
#include "GpuPhase.hpp"
#include "OamSearchPhase.hpp"
#include "PixelTransferPhase.hpp"
#include "ColourPalette.hpp"
#include "MemoryRegisters.hpp"
#include "Lcdc.hpp"

class Display;
class InterruptManager;
class Dma;

class Gpu final : public AddressSpace
{
public:
    enum class Mode
    {
        HBlank, VBlank, OamSearch, PixelTransfer, None //order is important!
    };

    Gpu(std::vector<std::uint8_t>&, Display&, InterruptManager&, Dma&, Ram&, MemoryRegisters&, bool);

    Ram& getVRam0();
    Ram& getVRam1();

    bool accepts(std::uint16_t) const override;
    void setByte(std::uint16_t, std::uint8_t) override;
    std::uint8_t getByte(std::uint16_t) const override;
    Mode tick();
    std::uint16_t getTicksInLine() const;
    Lcdc& getLcdc();
    //MemoryRegisters<GpuRegister>& getRegisters();
    bool isColour() const;
    ColourPalette& getBgPalette();
    Mode getMode() const;
    bool isLcdEnabled() const;

private:
    Display & m_display;
    InterruptManager& m_interruptManager;
    Dma& m_dma;
    Ram& m_oamRam;
    bool m_colour;

    Ram m_videoRam0;
    Ram m_videoRam1;
    Lcdc m_lcdc;
    ColourPalette m_bgPalette;
    ColourPalette m_oamPalette;

    MemoryRegisters& m_registers;

    HBlankPhase m_hblankPhase;
    VBlankPhase m_vblankPhase;
    OamSearchPhase m_oamSearchPhase;
    PixelTransferPhase m_transferPhase;
    GpuPhase* m_currentPhase;

    bool m_lcdEnabled;
    std::int16_t m_lcdEnableDelay;
    std::uint16_t m_ticksInLine;
    Mode m_currentMode;

    AddressSpace* getAddressSpace(std::uint16_t) const;
    void requestLcdInterrupt(std::uint8_t);
    void requestLycEqualsLyInterrupt(); //can you tell this was ported from java?
    std::uint8_t getStat() const;
    void setStat(std::uint8_t);
    void setLcdc(std::uint8_t);
    void disableLCD();
    void enableLCD();
};