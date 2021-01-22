#include "LPlotter.h"

#include <QDebug>

#define TIME_BETWEEN_FRAMES_MS  50

void expendRange(QCPRange &a_rRange)
{
    a_rRange.upper = a_rRange.upper>0 ? a_rRange.upper*1.1 : a_rRange.upper*0.9;
    a_rRange.lower = a_rRange.lower<0 ? a_rRange.lower*1.1 : a_rRange.lower*0.9;

    if(a_rRange.upper==0 && a_rRange.lower==0) {
        a_rRange.upper = 0.1;
        a_rRange.lower = -0.1;
    }
}

LPlotter::LPlotter(QWidget *parent)
    : QCustomPlot(parent)
{
#if 0
    this->setOpenGl(true);
#endif
    setNoAntialiasingOnDrag(true);

    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);

    axisRect(0)->removeAxis(xAxis);
    axisRect(0)->removeAxis(yAxis);
    axisRect(0)->removeAxis(xAxis2);
    axisRect(0)->removeAxis(yAxis2);
    m_pTimeAxis = new QCPAxis(axisRect(0), QCPAxis::AxisType::atBottom);
    m_pTimeAxis->setSelectableParts(QCPAxis::SelectablePart::spNone); // Not selectable
    axisRect(0)->addAxis(QCPAxis::AxisType::atBottom, m_pTimeAxis);
    axisRect(0)->setRangeZoomAxes(axisRect(0)->axes(QCPAxis::AxisType::atBottom));
    axisRect(0)->setRangeDragAxes(axisRect(0)->axes(QCPAxis::AxisType::atBottom));

    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(TIME_BETWEEN_FRAMES_MS);
    connect(m_pTimer, &QTimer::timeout, this, &LPlotter::refresh);

    m_pTimeAxis->setRange(0, 10);
    m_dMaxX = m_pTimeAxis->range().size();

    m_pCursorX1 = new LCursor(this);
    m_pCursorX2 = new LCursor(this);
    m_pCursorY1 = new LCursor(this);
    m_pCursorY2 = new LCursor(this);

    m_pCursorX1->setPen(QPen(Qt::DashLine));
    m_pCursorX1->setClipToAxisRect(true);
    m_pCursorX1->setClipAxisRect(axisRect(0));
    m_pCursorX2->setPen(QPen(Qt::DashLine));
    m_pCursorX2->setClipToAxisRect(true);
    m_pCursorX2->setClipAxisRect(axisRect(0));
    m_pCursorY1->setPen(QPen(Qt::DashLine));
    m_pCursorY1->setClipToAxisRect(true);
    m_pCursorY1->setClipAxisRect(axisRect(0));
    m_pCursorY2->setPen(QPen(Qt::DashLine));
    m_pCursorY2->setClipToAxisRect(true);
    m_pCursorY2->setClipAxisRect(axisRect(0));
}

LPlotter::~LPlotter()
{
    delete m_pTimer;
}

void LPlotter::setMaxX(double a_dValue)
{
    m_dMaxX = a_dValue;
}

float LPlotter::getMaxX() const
{
    return m_dMaxX;
}

void LPlotter::startRefresh(bool a_bStart)
{
    if(a_bStart) {
        m_pTimer->start();
    }
    else {
        m_pTimer->stop();
    }
}

void LPlotter::showCursor(bool a_bShow)
{
    m_bShowCursor = a_bShow;

    if(m_bShowCursor && axisRect(0)->axes(QCPAxis::AxisType::atLeft).size() > 0) {
        double maxY = axisRect(0)->axes(QCPAxis::AxisType::atLeft).first()->range().upper;
        double minY = axisRect(0)->axes(QCPAxis::AxisType::atLeft).first()->range().lower;
        double maxX = m_pTimeAxis->range().upper;
        double minX = m_pTimeAxis->range().lower;
        double x1 = minX + (maxX-minX)*1/3;
        double x2 = minX + (maxX-minX)*2/3;
        double y1 = minY + (maxY-minY)*1/3;
        double y2 = minY + (maxY-minY)*2/3;

        setCursorPosition(m_pCursorX1, x1, y1, x1, y2);
        setCursorPosition(m_pCursorX2, x2, y1, x2, y2);
        setCursorPosition(m_pCursorY1, x1, y1, x2, y1);
        setCursorPosition(m_pCursorY2, x1, y2, x2, y2);

        m_pCursorX1->setVisible(true);
        m_pCursorX2->setVisible(true);
        m_pCursorY1->setVisible(true);
        m_pCursorY2->setVisible(true);

        emit sendCursorInfo(x1, y1, x2, y2);
    }
    else {
        m_bShowCursor = false; // If no y axes
        m_pCursorX1->setVisible(false);
        m_pCursorX2->setVisible(false);
        m_pCursorY1->setVisible(false);
        m_pCursorY2->setVisible(false);
    }

    replot();
}

void LPlotter::showGrid(bool a_bShow)
{
    m_pTimeAxis->grid()->setVisible(a_bShow);
    if(axisRect(0)->axes(QCPAxis::AxisType::atLeft).size() > 0) {
        // Just protect, must satify
        axisRect(0)->axes(QCPAxis::AxisType::atLeft).first()->grid()->setVisible(a_bShow);
    }

    replot();
}

void LPlotter::addNewCurve(const QString &a_rName)
{
    if (!m_mapCurve.contains(a_rName)) {
        // Create a value axis for the new curve
        QCPAxis *pValueAxis = new QCPAxis(axisRect(0), QCPAxis::AxisType::atLeft);
        axisRect(0)->addAxis(QCPAxis::AxisType::atLeft, pValueAxis);
        axisRect(0)->setRangeZoomAxes(axisRect(0)->axes(QCPAxis::AxisType::atBottom),
                                      axisRect(0)->axes(QCPAxis::AxisType::atLeft));
        axisRect(0)->setRangeDragAxes(axisRect(0)->axes(QCPAxis::AxisType::atBottom),
                                      axisRect(0)->axes(QCPAxis::AxisType::atLeft));
        // Assign the axis
        m_mapCurve[a_rName] = new LCurve(m_pTimeAxis, pValueAxis);
        connect(m_mapCurve[a_rName], &LCurve::sendCurveSelected, this, &LPlotter::receiveCurveSelected);

    }
}

void LPlotter::addCurvePoint(const QString &a_rName, const QPointF &a_rPoint)
{
    if (m_mapCurve.contains(a_rName)) {
        m_mapCurve[a_rName]->addPoint(a_rPoint);
    }
}

void LPlotter::mergeValueAxis(const QList<QString> &a_rMergedName)
{
    if(a_rMergedName.size() > 1) {
        // If any curve not exist, do nothing
        foreach(QString strName, a_rMergedName) {
            if(!m_mapCurve.contains(strName)) {
                return;
            }
        }
        // Swap the first value axis to the nearest first
        QCPAxis *pFirst = m_mapCurve[a_rMergedName[0]]->getYAxis();
        if(pFirst != axisRect(0)->axes(QCPAxis::AxisType::atLeft).first()) {
            swapValueAxis(pFirst, axisRect(0)->axes(QCPAxis::AxisType::atLeft).first());
        }
        // Set the range to fit all curves
        bool foundRange;
        QCPRange range = m_mapCurve[a_rMergedName[0]]->getLine()->getValueRange(foundRange);
        foreach(QString strName, a_rMergedName) {
            range.expand(m_mapCurve[strName]->getLine()->getValueRange(foundRange));
        }
        // Expand range
        expendRange(range);
        axisRect(0)->axes(QCPAxis::AxisType::atLeft).first()->setRange(range);
        // Merge all the axis to the first value axis
        foreach(QString strName, a_rMergedName) {
            QCPAxis *pOldAxis = m_mapCurve[strName]->getYAxis();
            if(pOldAxis != nullptr && pOldAxis != axisRect(0)->axes(QCPAxis::AxisType::atLeft).first()) {
                m_mapCurve[strName]->getLine()->setValueAxis(axisRect(0)->axes(QCPAxis::AxisType::atLeft).first());
                axisRect(0)->removeAxis(pOldAxis);
            }
        }

        replot();
    }
}

void LPlotter::demergeValueAxis(const QList<QString> &a_rDemergedName)
{
    if(a_rDemergedName.size() > 1) {
        // If any curve not exist, or these curves don't have the same Y axis, do nothing
        QCPAxis *pOldAxis = m_mapCurve[a_rDemergedName[0]]->getYAxis();
        foreach(QString strName, a_rDemergedName) {
            if(!m_mapCurve.contains(strName)
                    || pOldAxis != m_mapCurve[strName]->getYAxis()) {
                return;
            }
        }

        foreach(QString strName, a_rDemergedName) {
            // Create a value axis for the new curve
            QCPAxis *pValueAxis = new QCPAxis(axisRect(0), QCPAxis::AxisType::atLeft);
            axisRect(0)->addAxis(QCPAxis::AxisType::atLeft, pValueAxis);
            axisRect(0)->setRangeZoomAxes(axisRect(0)->axes(QCPAxis::AxisType::atBottom),
                                          axisRect(0)->axes(QCPAxis::AxisType::atLeft));
            axisRect(0)->setRangeDragAxes(axisRect(0)->axes(QCPAxis::AxisType::atBottom),
                                          axisRect(0)->axes(QCPAxis::AxisType::atLeft));
            // Fit the range
            bool foundRange;
            QCPRange range = m_mapCurve[strName]->getLine()->getValueRange(foundRange);
            expendRange(range);
            pValueAxis->setRange(range);
            // Assign the axis
            m_mapCurve[strName]->getLine()->setValueAxis(pValueAxis);
        }

        axisRect(0)->removeAxis(pOldAxis);

        replot();
    }
}

void LPlotter::setCurveSelected(const QString &a_rName, bool a_bSelected)
{
    if (m_mapCurve.contains(a_rName)) {
        m_mapCurve[a_rName]->setSelected(a_bSelected);
        replot();
    }
}

void LPlotter::deleteAllCurve()
{
    QMapIterator<QString, LCurve*> i(m_mapCurve);
    while(i.hasNext()) {
        i.next();
        if (i.value() != nullptr) {
            delete i.value();
        }
    }
    m_mapCurve.clear();
}

void LPlotter::clearAllCurvePoints()
{
    QMapIterator<QString, LCurve*> i(m_mapCurve);
    while(i.hasNext()) {
        i.next();
        if (i.value() != nullptr) {
            i.value()->clear();
        }
    }
}

void LPlotter::deleteCurve(const QString &a_rName)
{
    if (m_mapCurve.contains(a_rName)) {
        if (m_mapCurve[a_rName] != nullptr) {
            // Remove the graph from plotter first
            //removeGraph(m_mapCurve[a_rName]->getLine());
            axisRect(0)->removeAxis(m_mapCurve[a_rName]->getYAxis());
            replot();
            delete m_mapCurve[a_rName]; m_mapCurve[a_rName] = nullptr;
        }
        m_mapCurve.remove(a_rName);
    }
}

void LPlotter::clearCurvePoints(const QString &a_rName)
{
    if (m_mapCurve.contains(a_rName)) {
        if (m_mapCurve[a_rName] != nullptr) {
            m_mapCurve[a_rName]->clear();
        }
    }
}

void LPlotter::setCurveStyle(const QString &a_rName, const Qt::PenStyle &a_rCurveStyle)
{

}

void LPlotter::setCurveColor(const QString &a_rName, const QColor &a_rColor)
{
    if (m_mapCurve.contains(a_rName)) {
        m_mapCurve[a_rName]->setColor(a_rColor);
        replot();
    }
}

void LPlotter::setCurveWeight(const QString &a_rName, int a_rCurveWeight)
{

}

void LPlotter::setCurveYGain(const QString &a_rName, float a_rCurveYGain)
{

}

void LPlotter::setCurveYOffset(const QString &a_rName, float a_fCurveYOffset)
{

}

void LPlotter::mousePressEvent(QMouseEvent *event)
{
    QCustomPlot::mousePressEvent(event);

    if(m_bShowCursor) {
        double distance = m_pCursorX1->selectTest(event->pos(), false);
        if(distance <= 5 && axisRect()->rect().contains(event->pos())) {
            m_pCursorX1->setStatus(LCursor::Status::ePressed);
        }
        distance = m_pCursorX2->selectTest(event->pos(), false);
        if(distance <= 5 && axisRect()->rect().contains(event->pos())) {
            m_pCursorX2->setStatus(LCursor::Status::ePressed);
        }
        distance = m_pCursorY1->selectTest(event->pos(), false);
        if(distance <= 5 && axisRect()->rect().contains(event->pos())) {
            m_pCursorY1->setStatus(LCursor::Status::ePressed);
        }
        distance = m_pCursorY2->selectTest(event->pos(), false);
        if(distance <= 5 && axisRect()->rect().contains(event->pos())) {
            m_pCursorY2->setStatus(LCursor::Status::ePressed);
        }
    }
}

void LPlotter::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bShowCursor
            && axisRect(0)->axes(QCPAxis::AxisType::atLeft).size() > 0
            && (m_pCursorX1->getStatus() == LCursor::Status::ePressed
                || m_pCursorX2->getStatus() == LCursor::Status::ePressed
                || m_pCursorY1->getStatus() == LCursor::Status::ePressed
                || m_pCursorY2->getStatus() == LCursor::Status::ePressed)) {
        QCPAxis *pValueAxis = axisRect(0)->axes(QCPAxis::AxisType::atLeft).first();
        if(m_pCursorX1->getStatus() == LCursor::Status::ePressed) {
            double dNewX = m_pTimeAxis->pixelToCoord(event->pos().x());
            double dOldY1 = m_pCursorX1->point1->value();
            double dOldY2 = m_pCursorX1->point2->value();
            setCursorPosition(m_pCursorX1, dNewX, dOldY1, dNewX, dOldY2);
            replot();
            emit sendCursorInfo(dNewX, dOldY1, m_pCursorX2->point1->key(), dOldY2);
        }
        if(m_pCursorX2->getStatus() == LCursor::Status::ePressed) {
            double dNewX = m_pTimeAxis->pixelToCoord(event->pos().x());
            double dOldY1 = m_pCursorX2->point1->value();
            double dOldY2 = m_pCursorX2->point2->value();
            setCursorPosition(m_pCursorX2, dNewX, dOldY1, dNewX, dOldY2);
            replot();
            emit sendCursorInfo(m_pCursorX1->point1->key(), dOldY1, dNewX, dOldY2);
        }
        if(m_pCursorY1->getStatus() == LCursor::Status::ePressed) {
            double dNewY = pValueAxis->pixelToCoord(event->pos().y());
            double dOldX1 = m_pCursorY1->point1->key();
            double dOldX2 = m_pCursorY1->point2->key();
            setCursorPosition(m_pCursorY1, dOldX1, dNewY, dOldX2, dNewY);
            replot();
            emit sendCursorInfo(dOldX1, dNewY, dOldX2, m_pCursorY2->point1->value());
        }
        if(m_pCursorY2->getStatus() == LCursor::Status::ePressed) {
            double dNewY = pValueAxis->pixelToCoord(event->pos().y());
            double dOldX1 = m_pCursorY2->point1->key();
            double dOldX2 = m_pCursorY2->point2->key();
            setCursorPosition(m_pCursorY2, dOldX1, dNewY, dOldX2, dNewY);
            replot();
            emit sendCursorInfo(dOldX1, m_pCursorY1->point1->value(), dOldX2, dNewY);
        }
    }
    else {
        QCustomPlot::mouseMoveEvent(event);
    }
}

void LPlotter::mouseReleaseEvent(QMouseEvent *event)
{
    QCustomPlot::mouseReleaseEvent(event);

    if(m_bShowCursor && axisRect(0)->axes(QCPAxis::AxisType::atLeft).size() > 0) {
        if(m_pCursorX1->getStatus() == LCursor::Status::ePressed) {
            m_pCursorX1->setStatus(LCursor::Status::eReleased);
        }
        if(m_pCursorX2->getStatus() == LCursor::Status::ePressed) {
            m_pCursorX2->setStatus(LCursor::Status::eReleased);
        }
        if(m_pCursorY1->getStatus() == LCursor::Status::ePressed) {
            m_pCursorY1->setStatus(LCursor::Status::eReleased);
        }
        if(m_pCursorY2->getStatus() == LCursor::Status::ePressed) {
            m_pCursorY2->setStatus(LCursor::Status::eReleased);
        }
    }
}

void LPlotter::refresh()
{
    double dSize = m_pTimeAxis->range().size();
    m_pTimeAxis->setRange(m_dMaxX-dSize, m_dMaxX);

    replot();
}

void LPlotter::receiveCurveSelected(bool selected)
{
    LCurve *pCurve = qobject_cast<LCurve*>(sender());
    if(pCurve) {
        if(selected) {
            if(axisRect(0)->axes(QCPAxis::AxisType::atLeft).size() > 1) {
                // At least 2 y axis, move the select one to the first
                QCPAxis *pOriginalValueAxis = pCurve->getYAxis();
                QCPAxis *pFirstValueAxis = axisRect(0)->axes(QCPAxis::AxisType::atLeft).first();
                swapValueAxis(pOriginalValueAxis, pFirstValueAxis);
            }
        }

        // Mark the axis selected or not
        if(selected) {
            pCurve->getYAxis()->setSelectedParts(QCPAxis::spAxis);
        }
        else {
            pCurve->getYAxis()->setSelectedParts(QCPAxis::spNone);
        }

        replot();
    }
}

void LPlotter::swapValueAxis(QCPAxis *a_pFirstAxis, QCPAxis *a_pSecondAxis)
{
    QCPRange firstRange = a_pFirstAxis->range();
    QCPRange secondRange = a_pSecondAxis->range();
    QList<QCPGraph*> listFirstAxisCurve = a_pFirstAxis->graphs();
    QList<QCPGraph*> listSecondAxisCurve = a_pSecondAxis->graphs();
    // Swap range
    a_pFirstAxis->setRange(secondRange);
    a_pSecondAxis->setRange(firstRange);
    // Swap all the curves belong to 2 axes
    foreach(QCPGraph* pGraph, listSecondAxisCurve) {
        pGraph->setValueAxis(a_pFirstAxis);
    }
    foreach(QCPGraph* pGraph, listFirstAxisCurve) {
        pGraph->setValueAxis(a_pSecondAxis);
    }
}

void LPlotter::setCursorPosition(QCPItemStraightLine *a_pCursor, double x1, double y1, double x2, double y2)
{
    a_pCursor->point1->setAxes(m_pTimeAxis, axisRect(0)->axes(QCPAxis::AxisType::atLeft).first());
    a_pCursor->point2->setAxes(m_pTimeAxis, axisRect(0)->axes(QCPAxis::AxisType::atLeft).first());
    a_pCursor->point1->setCoords(x1, y1);
    a_pCursor->point2->setCoords(x2, y2);
}
