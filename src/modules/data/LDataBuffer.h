#ifndef LDATABUFFER_H
#define LDATABUFFER_H

#include <QQueue>

#include "LDataValue.h"

#define MAX_BUFFER_SIZE 1024

class LDataBuffer
{
public:
    LDataBuffer();

    ~LDataBuffer();

    bool popValue(LDataValue &a_rValue);

    bool pushValue(const LDataValue &a_rValue);

private:
    QQueue<LDataValue> m_queBufferQueue;
};

#endif // LDATABUFFER_H
