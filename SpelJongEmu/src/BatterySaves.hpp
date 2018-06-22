/*
MIT License

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

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>

namespace Battery
{
    static void save(const std::vector<std::uint8_t>& data, const std::string& fileName)
    {
        std::ofstream file(fileName + ".sav", std::ios::out | std::ios::binary);
        if (file.good())
        {
            for (auto b : data) file << b;
        }
        else
        {
            std::cout << "Failed opening " << fileName << " for writing\n";
        }
        file.close();
    }

    static void load(std::vector<std::uint8_t>& data, const std::string& fileName)
    {
        std::ifstream file(fileName + ".sav", std::ios::in | std::ios::binary);
        if (file.good())
        {
            file.seekg(0, file.end);
            size_t length = static_cast<std::size_t>(file.tellg());
            file.seekg(0, file.beg);

            data.resize(length);

            file.read(reinterpret_cast<char*>(data.data()), length);
        }
    }
}