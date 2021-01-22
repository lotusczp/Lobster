#ifndef LSAVELOG_H
#define LSAVELOG_H

#include <QDockWidget>
#include <QTimer>

#include "LValueObserver.h"
#include "LDataLogHangler.h"

namespace Ui {
class LSaveLog;
}

class LSaveLog : public QDockWidget, public LValueObserver
{
    Q_OBJECT

public:
    explicit LSaveLog(QWidget *parent = nullptr);
    virtual ~LSaveLog() Q_DECL_OVERRIDE;

    //! inherit from LObserver
    virtual void updateValue(const QString& a_rDataName, const LDataValue& a_rDataValue) Q_DECL_OVERRIDE;

    //! inherit from LObserver
    virtual void start() Q_DECL_OVERRIDE;

    //! inherit from LObserver
    virtual void stop() Q_DECL_OVERRIDE;


private slots:
    void scanRows();

    void on_pushButtonStart_clicked();

    void on_pushButtonStop_clicked();

private:
    Ui::LSaveLog *ui;

    bool m_bIsObserverStarted;
    bool m_bIsSaving;

    LDataLogHangler* m_pDataLogHandler;     //!< log file handler

    QTimer *m_pScanTimer;                   //!< scan timer

    unsigned int m_uCurrentRowNumber;       //!< current row count
};

#endif // LSAVELOG_H
