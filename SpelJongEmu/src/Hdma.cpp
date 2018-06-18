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
    return (address >= HDMA1 && address <= HDMA5);
}

void Hdma::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    if (address == HDMA5)
    {
        if (m_inProgress && (address & (1 << 7)) == 0)
        {
            stopTransfer();
        }
        else
        {
            startTransfer(value);
        }
        return; //skip setting hdma5
    }
    setStorageValue(address, value);
}

std::uint8_t Hdma::getByte(std::uint16_t address) const
{
    assert(accepts(address));

    if (address == HDMA5)
    {
        return m_inProgress ? 0 : (1 << 7) | m_length;
    }

    return getStorageValue(address);//0xff
}

void Hdma::tick()
{
    if (!transferInProgress()) return;

    if (++m_tick < 0x20) return;

    for (auto i = 0; i < 0x10; ++i)
    {
        m_mmu.setByte(m_dst + i, m_mmu.getByte(m_src + i));
    }
    m_src += 0x10;
    m_dst += 0x10;

    if (m_length-- == 0)
    {
        m_inProgress = false;
        m_length = 0x7f;
    }
    else if(m_hblankTransfer)
    {
        m_gpuMode = Gpu::Mode::None;
    }
}

void Hdma::gpuUpdate(Gpu::Mode mode)
{
    m_gpuMode = mode;
}

void Hdma::onLcdSwitch(bool enabled)
{
    m_lcdEnabled = enabled;
}

bool Hdma::transferInProgress() const
{
    if (!m_inProgress) return false;

    if (m_hblankTransfer && (m_gpuMode == Gpu::Mode::HBlank || !m_lcdEnabled)) return true;

    if (!m_hblankTransfer) return true;

    return false;
}

//private
void Hdma::startTransfer(std::uint8_t reg)
{
    m_hblankTransfer = (reg & (1 << 7)) != 0;
    m_length = 0x7f & reg;

    m_src = (getStorageValue(HDMA1) << 8) | (getStorageValue(HDMA2) & 0xf0);
    m_dst = ((getStorageValue(HDMA3) & 0x1f) << 8) | (getStorageValue(HDMA4) & 0xf0);

    m_src &= 0xfff0;
    m_dst = (m_dst & 0x1fff) | 0x8000;

    m_inProgress = true;
}

void Hdma::stopTransfer()
{
    m_inProgress = false;
}