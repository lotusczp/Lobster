#ifndef LATTRIBUTESOBSERVER_H
#define LATTRIBUTESOBSERVER_H

#include "LAttributes.h"

class LAttributesObserver
{
public:
    LAttributesObserver() {}
    virtual ~LAttributesObserver() {}

    virtual void updateAttributes(const QString& a_rDataName, const LAttributes& a_rAttributes) = 0;
};

#endif // LATTRIBUTESOBSERVER_H
