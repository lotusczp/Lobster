#include "LDataBuffer.h"

LDataBuffer::LDataBuffer()
{
}

LDataBuffer::~LDataBuffer()
{
    m_queBufferQueue.clear();
}

bool LDataBuffer::popValue(LDataValue &a_rValue)
{
    if (m_queBufferQueue.isEmpty())
    {
        return false;
    }
    else
    {
        a_rValue = m_queBufferQueue.dequeue();
        return true;
    }
}

bool LDataBuffer::pushValue(const LDataValue &a_rValue)
{
    if (m_queBufferQueue.size() < MAX_BUFFER_SIZE)
    {
        m_queBufferQueue.enqueue(a_rValue);
        return true;
    }
    else
    {
        m_queBufferQueue.dequeue();
        m_queBufferQueue.enqueue(a_rValue);
        return false;
    }
}
