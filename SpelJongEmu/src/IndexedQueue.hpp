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

//implements a fixed size queue with random access

#include <array>
#include <cassert>

template <class T, std::size_t SIZE>
class IndexedQueue final
{
public:
    IndexedQueue() : m_size(0), m_offset(0) {}

    std::size_t size() const { return m_size; }

    void push(T value)
    {
        assert(m_size < m_array.size());
        m_array[(m_offset + m_size) % m_array.size()] = value;
        m_size++;
    }

    void pop()
    {
        assert(m_size != 0);
        m_size--;
        m_offset = (m_offset + 1) % m_array.size();
    }

    T front() const
    {
        return m_array[m_offset];
    }

    T back() const
    {
        return m_array[(m_offset + m_size) % m_array.size()];
    }

    T& operator[] (std::size_t index)
    {
        assert(index < m_size);
        return m_array[(m_offset + index) % m_array.size()];
    }

    T operator[](std::size_t index) const
    {
        assert(index < m_size);
        return m_array[(m_offset + index) % m_array.size()];
    }

    void clear()
    {
        m_size = 0;
        m_offset = 0;
    }

private:
    std::size_t m_size;
    std::size_t m_offset;
    std::array<T, SIZE> m_array;
};
