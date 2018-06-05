#pragma once

#include "AddressSpace.hpp"
#include "Gpu.hpp"

class Mmu;
class Hdma final : public AddressSpace
{
public:
    Hdma(std::vector<std::uint8_t>&, Mmu&);

    std::string getLabel() const override { return "HDMA"; }

    bool accepts(std::uint16_t) const override;

    void setByte(std::uint16_t, std::uint8_t) override;

    std::uint8_t getByte(std::uint16_t) const override;

    void tick();

    void gpuUpdate(Gpu::Mode);

    void onLcdSwitch(bool);

    bool transferInProgress() const;

private:
    Mmu& m_mmu;

    enum Address
    {
        HDMA1 = 0xff51,
        HDMA2,
        HDMA3,
        HDMA4,
        HDMA5
    };

    Gpu::Mode m_gpuMode;
    bool m_inProgress;
    bool m_hblankTransfer;
    bool m_lcdEnabled;

    std::uint8_t m_length;
    std::uint16_t m_src;
    std::uint16_t m_dst;
    std::int32_t m_tick;

    void startTransfer(std::uint8_t);

    void stopTransfer();
};