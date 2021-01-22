#ifndef LSELECTDATADLG_H
#define LSELECTDATADLG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QMap>

namespace Ui {
class LSelectDataDlg;
}

class LSelectDataDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LSelectDataDlg(QWidget *parent = 0);
    ~LSelectDataDlg();

    void setDataNameCollection(const QStringList& a_rDataNameCol);

    void loadHistory(const QList<QString>& a_rSelectNames);

    void clickOk();

    void selectName(const QString& a_rstrDataName);
    void unselectName(const QString& a_rstrDataName);


signals:
    void acceptSelection(QList<QString> a_listNames);


private slots:
    void on_lineEditKeyWord_textChanged(const QString &arg1);

    void on_lineEditKeyWordSelect_textChanged(const QString &arg1);

    void on_listWidgetCollection_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidgetSelected_itemDoubleClicked(QListWidgetItem *item);

    void on_buttonBox_accepted();

    void on_pushButtonAdd_clicked();

    void on_pushButtonAddAll_clicked();

    void on_pushButtonRemove_clicked();

    void on_pushButtonRemoveAll_clicked();

private:
    Ui::LSelectDataDlg *ui;

    QMap<QString, QList<QString> > m_mapTempStorage;    //!< [protocol][names of this protocol], this is for name list storage before quit this dialog
};

#endif // LSELECTDATADLG_H
