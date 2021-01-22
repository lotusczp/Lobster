#ifndef LCURVE_H
#define LCURVE_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QPen>

#include "qcustomplot.h"


class LCurve : public QObject
{
    Q_OBJECT
public:
    LCurve(QCPAxis *keyAxis, QCPAxis *valueAxis);
    virtual ~LCurve();

    // name
    void setName(const QString& a_rName);
    QString getName() const;

    //! add point to current twisted line
    void addPoint(const QPointF& a_rPoint);

    //! clear all points of all lines
    void clear();

    //! Set the color of this curve
    void setColor(const QColor& a_rColor);

    void setPenStyle(const Qt::PenStyle &a_rPenStyle);

    void setSelected(bool a_bSelected);

    QCPAxis* getXAxis() {return m_pLine->keyAxis();}

    QCPAxis* getYAxis() {return m_pLine->valueAxis();}


    QCPRange getXRange(bool &  foundRange) {return m_pLine->getKeyRange(foundRange);}

    QCPRange getYRange(bool &  foundRange) {return m_pLine->getValueRange(foundRange);}

    QCPGraph* getLine() {return m_pLine;}


signals:
    void sendCurveSelected(bool selected);


private:
    LCurve() {}

private slots:
    void receiveCurveSelected(bool a_bSelected);

private:
    QCPGraph                *m_pLine;
};

#endif // LCURVE_H
