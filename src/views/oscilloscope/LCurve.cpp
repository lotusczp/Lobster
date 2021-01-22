#include "LCurve.h"

LCurve::LCurve(QCPAxis *keyAxis, QCPAxis *valueAxis)
{
    m_pLine = new QCPGraph(keyAxis, valueAxis);
    connect(m_pLine, SIGNAL(selectionChanged(bool)), this, SLOT(receiveCurveSelected(bool)));
}

LCurve::~LCurve()
{
    //! \note   No need to delete m_pLine, its ownership belongs to QCustomPlot instance
}

void LCurve::setName(const QString &a_rName)
{
    m_pLine->setName(a_rName);
}

QString LCurve::getName() const
{
    return m_pLine->name();
}

void LCurve::addPoint(const QPointF &a_rPoint)
{
    m_pLine->addData(a_rPoint.x(), a_rPoint.y());
}

void LCurve::clear()
{
    m_pLine->setData(QVector<double>(), QVector<double>());
}

void LCurve::setColor(const QColor& a_rColor)
{
    QPen pen = m_pLine->pen();
    pen.setColor(a_rColor);
    m_pLine->setPen(pen);
#if 0
    pen = m_pLine->keyAxis()->basePen();
    pen.setColor(a_rColor);
    m_pLine->valueAxis()->setBasePen(pen);
    pen = m_pLine->keyAxis()->tickPen();
    pen.setColor(a_rColor);
    m_pLine->valueAxis()->setTickPen(pen);
    pen = m_pLine->keyAxis()->subTickPen();
    pen.setColor(a_rColor);
    m_pLine->valueAxis()->setSubTickPen(pen);
#endif
}

void LCurve::setPenStyle(const Qt::PenStyle &a_rPenStyle)
{
    QPen pen = m_pLine->pen();
    if(pen.style() != a_rPenStyle) {
        pen.setStyle(a_rPenStyle);
        m_pLine->setPen(pen);
    }
}

void LCurve::setSelected(bool a_bSelected)
{
    if(a_bSelected) {
        QCPDataRange range;
        // Select the last 2 points
        if(m_pLine->data()->size() >= 2) {
            range.setEnd(m_pLine->data()->size()-1);
            range.setBegin(m_pLine->data()->size()-2);
        }
        m_pLine->setSelection(QCPDataSelection(range));
    }
    else {
        m_pLine->setSelection(QCPDataSelection());
    }
}

void LCurve::receiveCurveSelected(bool a_bSelected)
{
    emit sendCurveSelected(a_bSelected);
}
