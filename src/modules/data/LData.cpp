#include "LValueObserver.h"
#include "LData.h"

LData::LData(const QString &a_rDataName)
{
    m_strName = a_rDataName;
}

LData::~LData()
{
    // do not delete the instance which is pointed by LObserver*
    m_listValueObv.clear();
}

QString LData::getDataName() const
{
    return m_strName;
}

void LData::notifyAllValueObs()
{
    LDataValue dataValue;
    while (this->popValue(dataValue)) {
        QListIterator<LValueObserver*> i(m_listValueObv);
        while(i.hasNext()) {
            i.next()->updateValue(m_strName, dataValue);
        }
    }
}

void LData::attachValueObserver(LValueObserver *a_pObserver)
{
    if ( !m_listValueObv.contains(a_pObserver) ) {
        m_listValueObv.append(a_pObserver);
    }
}

void LData::detachValueObserver(LValueObserver *a_pObserver)
{
    if ( m_listValueObv.contains(a_pObserver) ) {
        m_listValueObv.removeOne(a_pObserver);
    }
}

bool LData::pushValue(const LDataValue &a_rValue)
{
    return m_inputBuffer.pushValue(a_rValue);
}

bool LData::popValue(LDataValue &a_rValue)
{
    return m_inputBuffer.popValue(a_rValue);
}

void LData::notifyAllAttriObs()
{
    QListIterator<LAttributesObserver*> i(m_listAttriObv);
    while(i.hasNext()) {
        LAttributesObserver* pObs = i.next();
        pObs->updateAttributes(m_strName, m_attributes);
    }
}

void LData::attachAttriObserver(LAttributesObserver *a_pObserver)
{
    if(!m_listAttriObv.contains(a_pObserver)) {
        m_listAttriObv.append(a_pObserver);
    }
}

void LData::detachAttriObserver(LAttributesObserver *a_pObserver)
{
    if(m_listAttriObv.contains(a_pObserver)) {
        m_listAttriObv.removeOne(a_pObserver);
    }
}

void LData::setAttributes(const LAttributes &a_rAttributes)
{
    m_attributes = a_rAttributes;
}

const LAttributes &LData::getAttributes() const
{
    return m_attributes;
}
