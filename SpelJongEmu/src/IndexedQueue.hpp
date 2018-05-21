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
