#include "Apu.hpp"
#include "AudioChannelOne.hpp"
#include "AudioChannelTwo.hpp"
#include "AudioChannelThree.hpp"
#include "AudioChannelFour.hpp"

namespace
{
    const std::uint16_t AddressOffset = 0xff10;
    //const std::uint16_t AddressSize = 0xff27 - AddressOffset;

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
    : AddressSpace  (storage),
    m_enabled       (false)
{
    m_channelGenerators[0] = std::make_unique<ChannelOne>(storage);
    m_channelGenerators[1] = std::make_unique<ChannelTwo>(storage);
    m_channelGenerators[2] = std::make_unique<ChannelThree>(storage);
    m_channelGenerators[3] = std::make_unique<ChannelFour>(storage);


    //map all the channels to the address space
    for (auto i = 0u; i < m_addressMap.size(); ++i)
    {
        auto address = AddressOffset + i;
        m_addressMap[i] = nullptr;

        for (auto& c : m_channelGenerators)
        {
            if (c->accepts(address))
            {
                m_addressMap[i] = c.get();
                break;
            }
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
            //TODO clear all
            //sound register values
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

    //if (m_enabled) return;

    auto relAddress = address - AddressOffset;
    if (m_addressMap[relAddress])
    {
        m_addressMap[relAddress]->setByte(address, value);
    }
    else
    {
        setStorageValue(address, value);
    }

    //std::cout << "write " << std::hex << address << "\n";
}

std::uint8_t Apu::getByte(std::uint16_t address) const
{
    //std::cout << "read " << std::hex << address << "\n";

    assert(accepts(address));
    std::uint8_t retVal = 0;

    auto relAddress = address - AddressOffset;
    if (address == 0xff26)
    {
        for (auto i = 0; i < m_channelGenerators.size(); ++i)
        {
            retVal |= m_channelGenerators[i]->isEnabled() ? (1 << i) : 0;
        }

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
    if (!m_enabled)
    {
        //m_output.addSample(0,0);
        return;
    }
 
    for (auto i = 0; i < m_channelGenerators.size(); ++i)
    {
        m_channelOutputs[i] = m_channelGenerators[i]->tick();
    }

    auto channelSelection = getStorageValue(0xff25);
    std::int32_t left = 0;
    std::int32_t right = 0;

    for (auto i = 0; i < 4; ++i)
    {
        if (channelSelection & (1 << i))
        {
            right += m_channelOutputs[i];
        }

        if (channelSelection & (1 << (i + 4)))
        {
            left += m_channelOutputs[i];
        }
    }
    left /= 4;
    right /= 4;

    auto volumes = getStorageValue(0xff24);
    right *= (volumes & 0x7);
    left *= ((volumes >> 4) & 0x7);

    m_output.addSample(static_cast<std::uint8_t>(/*rand() % 128*/left), static_cast<std::uint8_t>(right));
}

void Apu::enableChannel(bool enable, std::int32_t channel)
{
    m_overrideEnabled[channel] = enable;
}

void Apu::enableColour(bool enable)
{
    for (auto& c : m_channelGenerators)
    {
        c->setColour(enable);
    }
}

//private
void Apu::start()
{
    for (auto i = 0xff10; i < 0xff26; ++i)
    {
        if (i == 0xff11 || i == 0xff16 || i == 0xff1b || i == 0xff20)
        {
            //preserve length
            continue;
        }
        setByte(i, 0);
    }

    for (auto& c : m_channelGenerators)
    {
        c->start();
    }

    m_output.start();
}

void Apu::stop()
{
    for (auto& c : m_channelGenerators)
    {
        c->stop();
    }
    m_output.stop();
}