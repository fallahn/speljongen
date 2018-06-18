#include "Apu.hpp"

namespace
{
    const std::uint16_t AddressOffset = 0xff10;
    const std::uint16_t AddressSize = 0xff26 - AddressOffset;

    const std::array<std::uint8_t, 48u> Masks = 
    {
        0x80, 0x3f, 0x00, 0xff, 0xbf,
        0xff, 0x3f, 0x00, 0xff, 0xbf,
        0x7f, 0xff, 0x9f, 0xff, 0xbf,
        0xff, 0xff, 0x00, 0x00, 0xbf,
        0x00, 0x00, 0x70,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
}

Apu::Apu(std::vector<std::uint8_t>& storage)
    : AddressSpace(storage),
    m_enabled(false),
    m_channelOne(storage)
{
    //map all the channels to the address space
    for (auto i = 0u; i < AddressSize; ++i)
    {
        auto address = AddressOffset + i;
        if (m_channelOne.accepts(address))
        {
            m_addressMap[i] = &m_channelOne;
        }
        //TODO other channels
        else
        {
            m_addressMap[i] = nullptr;
        }
    }
}

//public
bool Apu::accepts(std::uint16_t address) const
{
    return address >= 0xff10 && address < 0xff27;
}

void Apu::setByte(std::uint16_t address, std::uint8_t value)
{
    assert(accepts(address));
    if (address == 0xff26)
    {
        if ((value & (1 << 7)) == 0)
        {
            if (m_enabled)
            {
                m_enabled = false;
                stop();
            }
            else
            {
                if (!m_enabled)
                {
                    m_enabled = true;
                    start();
                }
            }
        }
    }

    auto relAddress = address - AddressOffset;
    if (m_addressMap[relAddress])
    {
        m_addressMap[relAddress]->setByte(address, value);
    }
    else
    {
        setStorageValue(address, value);
    }
}

std::uint8_t Apu::getByte(std::uint16_t address) const
{
    assert(accepts(address));
    std::uint8_t retVal = 0;

    auto relAddress = address - AddressOffset;
    if (address == 0xff26)
    {
        retVal |= m_channelOne.isEnabled() ? (1 << 0) : 0;
        //TODO other channels

        retVal |= m_enabled ? (1 << 7) : 0;
    }
    else
    {
        if (m_addressMap[relAddress])
        {
            retVal = m_addressMap[relAddress]->getByte(address);
        }
        else
        {
            retVal = getStorageValue(address);
        }
    }
    return retVal | Masks[relAddress];
}

void Apu::tick()
{
    if (!m_enabled) return;

    m_channels[0] = m_channelOne.tick();
    //TODO other channels


}

void Apu::enableChannel(bool enable, std::int32_t channel)
{

}

void Apu::enableColour(bool enable)
{
    m_channelOne.setColour(enable);
}

//private
void Apu::start()
{

}

void Apu::stop()
{

}