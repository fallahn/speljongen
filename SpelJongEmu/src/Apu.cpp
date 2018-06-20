#include "Apu.hpp"

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
    m_enabled       (false),
    m_channelOne    (storage)
{
    //map all the channels to the address space
    for (auto i = 0u; i < m_addressMap.size(); ++i)
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

    //m_output.play();
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
    if (!m_enabled)
    {
        m_output.addSample(127, 127);
        return;
    }
 
    m_channels[0] = m_channelOne.tick();
    //TODO other channels


    auto channelSelection = getStorageValue(0xff25);
    std::int32_t left = 0;
    std::int32_t right = 0;

    for (auto i = 0; i < 4; ++i)
    {
        if (channelSelection & (1 << i))
        {
            right += m_channels[i];
        }

        if (channelSelection & (1 << (i + 4)))
        {
            left += m_channels[i];
        }
    }
    left /= 4;
    right /= 4;

    auto volumes = getStorageValue(0xff24);
    right *= (volumes & 0x7);
    left *= ((volumes >> 4) & 0x7);

    m_output.addSample(static_cast<std::uint8_t>(/*rand() % 128*/left), static_cast<std::uint8_t>(right));
    //if(left > 0)std::cout << left << "\n";
}

void Apu::enableChannel(bool enable, std::int32_t channel)
{
    m_overrideEnabled[channel] = enable;
}

void Apu::enableColour(bool enable)
{
    m_channelOne.setColour(enable);
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


    m_channelOne.start();


    //if (m_output.getStatus() != sf::SoundStream::Playing)
    //{
    //    m_output.play();
    //    //m_output.setLoop(true);
    //}
}

void Apu::stop()
{
    //m_output.stop();
    m_channelOne.stop();
}