#include "LSettingsTabDialog.h"
#include "ui_LSettingsTabDialog.h"

LSettingsTabDialog::LSettingsTabDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LSettingsTabDialog)
{
    ui->setupUi(this);
}

LSettingsTabDialog::~LSettingsTabDialog()
{
    delete ui;
}

int LSettingsTabDialog::addTab(QWidget *page, const QString &label)
{
    if (ui->tabWidget->indexOf(page) == -1) {
        // this widget is new
        return ui->tabWidget->addTab(page, label);
    }
    else {
        // this widget already exists
        return (-1);
    }
}

int LSettingsTabDialog::count() const
{
    return ui->tabWidget->count();
}
