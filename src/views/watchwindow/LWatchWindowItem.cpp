#include "LWatchWindowItem.h"

LWatchWindowItem::LWatchWindowItem(const QString &a_rDataName) :
    m_strDataName(a_rDataName),
    m_strValue("NA"),
    m_bScopeEnabled(false),
    m_curveColor(255,0,0),
    m_bLogEnabled(false)
{

}

const QString &LWatchWindowItem::getName() const
{
    return m_strDataName;
}

const QString &LWatchWindowItem::getValue() const
{
    return m_strValue;
}

void LWatchWindowItem::setValue(const QString &a_strValue)
{
    m_strValue = a_strValue;
}

bool LWatchWindowItem::isScopeEnable() const
{
    return m_bScopeEnabled;
}

void LWatchWindowItem::setScopeEnable(bool a_bEnable)
{
    m_bScopeEnabled = a_bEnable;
}

const QColor &LWatchWindowItem::getCurveColor() const
{
    return m_curveColor;
}

void LWatchWindowItem::setCurveColor(const QColor &a_rColor)
{
    m_curveColor = a_rColor;
}

bool LWatchWindowItem::isLogEnable() const
{
    return m_bLogEnabled;
}

void LWatchWindowItem::setLogEnable(bool a_bEnable)
{
    m_bLogEnabled = a_bEnable;
}

LWatchWindowItem &LWatchWindowItem::operator=(const LWatchWindowItem &other)
{
    m_strDataName   =   other.m_strDataName;
    m_bScopeEnabled =   other.m_bScopeEnabled;
    m_curveColor    =   other.m_curveColor;
    m_bLogEnabled   =   other.m_bLogEnabled;

    return *this;
}

void LWatchWindowItem::getScopeCheckState(bool a_bState)
{
    m_bScopeEnabled = a_bState;
    emit sendScopeEnableStateByItem(m_strDataName, a_bState);
}

void LWatchWindowItem::getLogCheckState(bool a_bState)
{
    m_bLogEnabled = a_bState;
    emit sendLogEnableStateByItem(m_strDataName, a_bState);
}

void LWatchWindowItem::getColorChanged(QColor color)
{
    m_curveColor = color;
    emit sendCurveColorChangedByItem(m_strDataName);
}

LWatchWindowItem::LWatchWindowItem()
{
}
