#include "InterruptManager.hpp"

InterruptManager::InterruptManager(std::vector<std::uint8_t>&storage, bool isColour)
    : AddressSpace              (storage),
    m_isColour                  (isColour),
    m_ime                       (false),
    m_interruptFlag             (0xe1),
    m_interruptEnabled          (0),
    m_pendingEnableInterrupts   (-1),
    m_pendingDisableInterrupts  (-1)
{

}

//public
void InterruptManager::enableInterrupts(bool useDelay)
{
    if (useDelay)
    {
        m_pendingEnableInterrupts = 1;
        m_pendingDisableInterrupts = -1;
    }
    else
    {
        m_ime = true;
    }
}

void InterruptManager::disableInterrupts(bool useDelay)
{
    if (useDelay && m_isColour)
    {
        m_pendingDisableInterrupts = 1;
        m_pendingEnableInterrupts = -1;
    }
    else
    {
        m_ime = false;
    }
}

void InterruptManager::requestInterrupt(Interrupt::Type interrupt)
{
    m_interruptFlag |= (1 << interrupt);
}

void InterruptManager::onInstructionFinished()
{
    if (m_pendingEnableInterrupts != -1)
    {
        if (m_pendingEnableInterrupts-- == 0)
        {
            enableInterrupts(false);
        }
    }

    if (m_pendingDisableInterrupts != -1)
    {
        if (m_pendingDisableInterrupts-- == 0)
        {
            disableInterrupts(false);
        }
    }
}

bool InterruptManager::isIME() const
{
    return m_ime;
}

bool InterruptManager::isInterruptRequested() const
{
    return (m_interruptEnabled & m_interruptFlag) != 0;
}

void InterruptManager::flush()
{
    m_interruptFlag = 0xe0; //not 0xe1?
}

bool InterruptManager::isHaltBug() const
{
    return ((m_interruptFlag & m_interruptEnabled) & 0x1f) != 0 && !m_ime;
}

bool InterruptManager::accepts(std::uint16_t address) const
{
    return address == 0xff0f || address == 0xffff;
}

void InterruptManager::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    switch (address)
    {
    default: return;
    case 0xff0f:
        m_interruptFlag = (value | 0xe0); //why switch on top bits?
        setStorageValue(address, m_interruptFlag);
        break;
    case 0xffff:
        m_interruptEnabled = value;
        setStorageValue(address, value);
        break;
    }
}

std::uint8_t InterruptManager::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    switch (address)
    {
    default: return 0xff;
    case 0xff0f:
        return m_interruptFlag;
    case 0xffff:
        return m_interruptEnabled;
    }
}