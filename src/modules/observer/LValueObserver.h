#ifndef LVALUEOBSERVER_H
#define LVALUEOBSERVER_H

#include "LDataValue.h"

class LValueObserver
{
public:
    LValueObserver() {}
    virtual ~LValueObserver() {}

    virtual void updateValue(const QString& a_rDataName, const LDataValue& a_rDataValue) = 0;

    virtual void start() = 0;

    virtual void stop() = 0;
};

#endif // LVALUEOBSERVER_H
