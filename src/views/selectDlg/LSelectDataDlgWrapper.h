#ifndef LSELECTDATAWRAPPER_H
#define LSELECTDATAWRAPPER_H

#include "LSettingsRelier.h"
#include "LSelectDataDlg.h"


class LSelectDataDlgWrapper : public QObject, public LSettingsRelier
{
    Q_OBJECT
public:
    LSelectDataDlgWrapper();
    ~LSelectDataDlgWrapper();

    void setDataNameCollection(const QStringList& a_rDataNameCol);

    void loadHistory();

    void selectName(const QString& a_rstrDataName);
    void unselectName(const QString& a_rstrDataName);


    void createDialog(QWidget* parent);
    void execDialog();

    //! an external command to click ok
    void clickOk();

signals:
    void triggerAcceptSelection(QList<QString> a_listNames);

public slots:
    void getAcceptSig(QList<QString> a_listNames);

protected:
    virtual void parseFromSetting(LObixTreeIter a_obixIter) Q_DECL_OVERRIDE;

    virtual void convertToSetting() Q_DECL_OVERRIDE;


private:
    LSelectDataDlg* m_pDialog;

    QList<QString> m_listHistoryNames;
};

#endif // LSELECTDATAWRAPPER_H
