#include "LAddNewChannelDlg.h"
#include "ui_LAddNewChannelDlg.h"

#include <QMessageBox>

LAddNewChannelDlg::LAddNewChannelDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LAddNewChannelDlg)
{
    ui->setupUi(this);
}

LAddNewChannelDlg::~LAddNewChannelDlg()
{
    delete ui;
}

void LAddNewChannelDlg::on_buttonBox_accepted()
{
    if ((ui->lineEditName->text() != "") && (ui->lineEditConfigPath->text() != "") && (ui->lineEditPluginPath->text() != "")) {
        emit sendNewChannel(ui->lineEditName->text(), ui->lineEditConfigPath->text(), ui->lineEditPluginPath->text());
    }
    else {
        if (ui->lineEditName->text() == "") {
            QMessageBox::warning(0, QObject::tr("Warning"), ui->labelName->text()+" can not be empty");
        }
        if (ui->lineEditConfigPath->text() == "") {
            QMessageBox::warning(0, QObject::tr("Warning"), ui->labelConfigPath->text()+" can not be empty");
        }
        if (ui->lineEditPluginPath->text() == "") {
            QMessageBox::warning(0, QObject::tr("Warning"), ui->lineEditPluginPath->text()+" can not be empty");
        }
    }
}
