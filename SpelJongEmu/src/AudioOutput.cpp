#include "AudioOutput.hpp"

#include <SFML/System/Lock.hpp>

AudioOutput::AudioOutput()
    : m_bufferSize(0)
{
    m_buffer.resize(256);
    m_outBuffer.resize(256);

    initialize(2, 22050); //TODO how do we set 8 bit samples?
}

//public
void AudioOutput::addSample(std::uint8_t left, std::uint8_t right)
{
    //TODO we have to normalise as float then convert to 16bit...


    sf::Lock lock(m_mutex);
    m_buffer[m_bufferSize++] = left;
    m_buffer[m_bufferSize++] = right;
}

//private
bool AudioOutput::onGetData(Chunk& chunk)
{
    if (m_bufferSize)
    {
        m_mutex.lock();
        chunk.sampleCount = m_bufferSize;
        m_bufferSize = 0;
        m_buffer.swap(m_outBuffer);
        m_mutex.unlock();

        chunk.samples = m_outBuffer.data();
        return true;
    }
    return false;
}