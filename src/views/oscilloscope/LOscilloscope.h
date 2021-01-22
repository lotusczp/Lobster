#ifndef LOSCILLOSCOPE_H
#define LOSCILLOSCOPE_H

#include <QDockWidget>

#include "LValueObserver.h"

#include "LPlotter.h"
#include "LScopeMenu.h"

namespace Ui {
class LOscilloscope;
}

class LOscilloscope : public QDockWidget, public LValueObserver
{
    Q_OBJECT

public:
    explicit LOscilloscope(QWidget *parent = 0);
    ~LOscilloscope();


    //! inherit from LObserver
    virtual void updateValue(const QString& a_rDataName, const LDataValue& a_rDataValue) Q_DECL_OVERRIDE;

    //! inherit from LObserver
    virtual void start() Q_DECL_OVERRIDE;

    //! inherit from LObserver
    virtual void stop() Q_DECL_OVERRIDE;


    void addNewCurve(const QString& a_rDataName);

    void eraseCurve(const QString& a_rDataName);


    void setCurveColor(const QString &a_rName, const QColor &a_rColor);

    void setCurveSelected(const QString &a_rName, bool a_bSelected);

    void mergeValueAxis(QList<QString> a_listMergedName);

    void demergeValueAxis(QList<QString> a_listName);

    void setCurveYGain(const QString &a_rName, float a_fCurveYGain);

    void setCurveYOffset(const QString &a_rName, float a_fCurveYOffset);

signals:
    void curveSelected();

private:
    Ui::LOscilloscope *ui;

    qint64 m_iScopeStartTime;

    bool m_bIsStarted;
};

#endif // LOSCILLOSCOPE_H
