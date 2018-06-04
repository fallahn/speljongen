#include "Hdma.hpp"
#include "Mmu.hpp"

Hdma::Hdma(std::vector<std::uint8_t>& storage, Mmu& mmu)
    : AddressSpace  (storage),
    m_mmu           (mmu),
    m_gpuMode       (Gpu::Mode::None),
    m_inProgress    (false),
    m_hblankTransfer(false),
    m_lcdEnabled    (false),
    m_length        (0),
    m_src           (0),
    m_dst           (0),
    m_tick          (0)
{

}

//public
bool Hdma::accepts(std::uint16_t address) const
{
    return false;
}

void Hdma::setByte(std::uint16_t address, std::uint8_t value)
{

}

std::uint8_t Hdma::getByte(std::uint16_t address) const
{
    return 0;
}

void Hdma::tick()
{

}

void Hdma::gpuUpdate(Gpu::Mode mode)
{

}

void Hdma::onLcdSwitch(bool enabled)
{

}

bool Hdma::transferInProgress() const
{
    return false;
}

//private
void Hdma::startTransfer(std::uint8_t register)
{

}

void Hdma::stopTransfer()
{

}