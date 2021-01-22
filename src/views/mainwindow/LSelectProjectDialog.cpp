#include "LSelectProjectDialog.h"
#include "ui_LSelectProjectDialog.h"

#include <QFileDialog>

LSelectProjectDialog::LSelectProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LSelectProjectDialog)
{
    ui->setupUi(this);
}

LSelectProjectDialog::~LSelectProjectDialog()
{
    delete ui;
}

QString LSelectProjectDialog::getProjectPath() const
{
    return ui->filePathEdit->text();
}

void LSelectProjectDialog::on_browseButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Directory"),
                                                    tr("./projects"),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->filePathEdit->setText(dir);
}
