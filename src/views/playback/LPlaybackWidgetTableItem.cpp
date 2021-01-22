#include "LPlaybackWidgetTableItem.h"

LPlaybackWidgetTableItem::LPlaybackWidgetTableItem(const QString &a_rDataName) :
    m_strDataName(a_rDataName),
    m_curveColor(255,0,0)
{

}

LPlaybackWidgetTableItem &LPlaybackWidgetTableItem::operator=(const LPlaybackWidgetTableItem &other)
{
    m_strDataName   =   other.m_strDataName;
    m_curveColor    =   other.m_curveColor;

    return *this;
}

void LPlaybackWidgetTableItem::getColorChanged(QColor color)
{
    m_curveColor = color;
    emit sendCurveColorChangedByItem(m_strDataName);
}

LPlaybackWidgetTableItem::LPlaybackWidgetTableItem()
{

}
