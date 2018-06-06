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

#include <array>

using Palette = std::array<std::uint16_t, 4u>;
class ColourPalette final : public AddressSpace 
{
public:
    ColourPalette(std::vector<std::uint8_t>& storage, std::uint16_t offset);

    bool accepts(const std::uint16_t address) const override;

    void setByte(std::uint16_t, std::uint8_t) override;

    std::uint8_t getByte(std::uint16_t) const override;

    const Palette& getPalette(std::size_t) const;

    void clear();

    std::string getLabel() const override { return "Colour Palette"; }

private:
    std::array<Palette, 8u>  m_palettes; //TODO these need to be mapped to storage
    std::uint16_t m_indexAddress;
    std::uint16_t m_dataAddress;
    bool m_autoIncrement;
};
