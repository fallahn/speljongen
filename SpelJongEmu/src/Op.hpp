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

#include <cstdint>
#include <cstddef>

#include <array>
#include <functional>

class AddressSpace;
class Registers;
class InterruptManager;

using OpArgs = std::array<std::uint8_t, 2>;
using OpExecution = std::function<std::uint16_t(Registers&, AddressSpace&, const OpArgs&, std::uint16_t)>;

struct Op final
{
    bool readsMemory = false;
    bool writesMemory = false;
    std::size_t operandLength = 0;

    OpExecution execute =
        [](Registers&, AddressSpace&, const OpArgs&, std::uint16_t context)
    {
        return context;
    };

    std::function<void(InterruptManager& interruptManager)> switchInterrupts = [](InterruptManager&) {};
    std::function<bool(Registers& registers)>proceed = [](Registers&) { return true; };
    bool forceFinishCycle = false;
};