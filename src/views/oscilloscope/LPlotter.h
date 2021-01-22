#ifndef LPLOTTER_H
#define LPLOTTER_H

#include <QMap>
#include <QTimer>

#include "LPlotSettings.h"
#include "LCurve.h"
#include "qcustomplot.h"
#include "LCursor.h"


class LPlotter : public QCustomPlot
{
    Q_OBJECT

public:
    LPlotter(QWidget *parent = nullptr);
    virtual ~LPlotter();

    void setMaxX(double a_dValue);

    float getMaxX() const;

    void startRefresh(bool a_bStart);

    void showCursor(bool a_bShow);

    void showGrid(bool a_bShow);


    void addNewCurve(const QString& a_rName);

    void addCurvePoint(const QString& a_rName, const QPointF &a_rPoint);

    void mergeValueAxis(const QList<QString> &a_rMergedName);

    void demergeValueAxis(const QList<QString> &a_rDemergedName);

    void setCurveSelected(const QString& a_rName, bool a_bSelected);


    void deleteAllCurve();

    void clearAllCurvePoints();

    void deleteCurve(const QString& a_rName);

    void clearCurvePoints(const QString& a_rName);


    void setCurveStyle(const QString& a_rName, const Qt::PenStyle& a_rCurveStyle);

    void setCurveColor(const QString &a_rName, const QColor &a_rColor);

    void setCurveWeight(const QString &a_rName, int a_iCurveWeight);

    void setCurveYGain(const QString &a_rName, float a_fCurveYGain);

    void setCurveYOffset(const QString &a_rName, float a_fCurveYOffset);

signals:
    void curveSelected(QString name, bool selected);

    void sendCursorInfo(double x1, double y1, double x2, double y2);

protected:
    virtual void mousePressEvent(QMouseEvent * event);

    virtual void mouseMoveEvent(QMouseEvent *event);

    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    void refresh();

    void receiveCurveSelected(bool selected);

    void swapValueAxis(QCPAxis* a_pFirstAxis, QCPAxis* a_pSecondAxis);

    void setCursorPosition(QCPItemStraightLine *a_pCursor, double x1, double y1, double x2, double y2);

private:
    double      m_dMaxX;

    QCPAxis     *m_pTimeAxis;

    QTimer      *m_pTimer;

    bool        m_bShowCursor;

    QMap<QString, LCurve*> m_mapCurve;  //!< [curve name] [curve]

    LCursor *m_pCursorX1;
    LCursor *m_pCursorX2;
    LCursor *m_pCursorY1;
    LCursor *m_pCursorY2;
};

#endif // LPLOTTER_H
