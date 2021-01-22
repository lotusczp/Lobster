#ifndef LCURSOR_H
#define LCURSOR_H

#include "qcustomplot.h"

class LCursor : public QCPItemStraightLine
{
    Q_OBJECT
public:
    enum Status
    {
        ePressed,
        eReleased,
    };

public:
    explicit LCursor(QCustomPlot *parentPlot);

    void setStatus(Status a_eStatus)
    {
        m_eStatus = a_eStatus;
    }

    Status getStatus() const
    {
        return m_eStatus;
    }

signals:

private:
    Status m_eStatus;
};

#endif // LCURSOR_H
