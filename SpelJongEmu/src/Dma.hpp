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
#include "SpeedMode.hpp"

#include <cassert>

class Dma final : public AddressSpace
{
public:
    Dma(std::vector<std::uint8_t>& storage, const SpeedMode& speedMode)
        : AddressSpace(storage), m_speedMode(speedMode),
        m_inProgress(false), m_restarted(false), m_ticks(0)
    {
        setStorageValue(0xff46, 0);
    }

    std::string getLabel() const override { return "DMA"; }

    bool accepts(std::uint16_t address) const override
    {
        return address == 0xff46;
    }

    std::uint8_t getByte(std::uint16_t address) const override 
    { 
        assert(accepts(address));
        return getStorageValue(address);
    }

    void setByte(std::uint16_t address, std::uint8_t value) override
    {
        assert(accepts(address));
        setStorageValue(address, value);
        m_restarted = isOamBlocked();
        m_ticks = 0;
        m_inProgress = true;
    }

    bool isOamBlocked() const
    {
        return m_restarted || (m_inProgress && m_ticks > 4);
    }

    void tick()
    {
        if (m_inProgress)
        {
            if (++m_ticks >= (648 / m_speedMode.getSpeedMode()))
            {
                m_inProgress = false;
                m_restarted = false;
                m_ticks = 0;

                //do transfer
                std::uint16_t from = getStorageValue(0xff46) * 0x100;
                for (std::uint16_t i = 0; i < 0xa0; ++i)
                {
                    setStorageValue(0xfe00 + i, getStorageValue(from + i));
                }
            }
        }
    }

private:
    const SpeedMode& m_speedMode;

    bool m_inProgress;
    bool m_restarted;

    std::uint32_t m_ticks;
};