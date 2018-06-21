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

//circular buffer wrapped around std::vector

#include <vector>
#include <cassert>

template <class T>
class RingBuffer final
{
public:

    T read()
    {
        assert(!m_data.empty());
        auto ret = m_data[m_out];
        m_out = (m_out + 1) % m_data.size();
        return ret;
    }

    void write(const T& v)
    {
        assert(!m_data.empty());
        m_data[m_in] = v;
        m_in = (m_in + 1) % m_data.size();
    }

    T back() const
    {
        assert(!m_data.empty());
        return m_data[m_out];
    }

    std::size_t size() const
    {
        return m_data.size();
    }

    void resize(std::size_t size, const T& value = {})
    {
        m_data.resize(size);
        for (auto& v : m_data) v = value;
        m_in = 0;
        m_out = 0;
    }

    bool pending() const
    {
        return m_in != m_out;
    }

    void reset()
    {
        m_data.clear();
        m_in = 0;
        m_out = 0;
    }

    const T& operator [] (std::size_t idx) const
    {
        assert(!m_data.empty());
        return m_data[idx];
    }

    T& operator [] (std::size_t idx)
    {
        assert(!m_data.empty());
        return m_data[idx];
    }

    operator bool() const
    {
        return !m_data.empty();
    }

private:
    std::vector<T> m_data;
    std::size_t m_in = 0;
    std::size_t m_out = 0;
};