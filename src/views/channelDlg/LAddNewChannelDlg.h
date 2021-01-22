#ifndef LADDNEWCHANNELDLG_H
#define LADDNEWCHANNELDLG_H

#include <QDialog>

namespace Ui {
class LAddNewChannelDlg;
}

class LAddNewChannelDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LAddNewChannelDlg(QWidget *parent = 0);
    ~LAddNewChannelDlg();

signals:
    void sendNewChannel(QString a_strChannelName, QString a_strConfigPath, QString a_strPluginPath);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::LAddNewChannelDlg *ui;
};

#endif // LADDNEWCHANNELDLG_H
