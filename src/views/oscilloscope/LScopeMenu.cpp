#include "LScopeMenu.h"
#include "ui_LScopeMenu.h"

LScopeMenu::LScopeMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LScopeMenu)
{
    ui->setupUi(this);

    m_strRegisterName = "ScopeMenu";

}

LScopeMenu::~LScopeMenu()
{
    delete ui;
}

void LScopeMenu::receiveCursorInfo(double x1, double y1, double x2, double y2)
{
    ui->lineEditX1->setText(QString::number(x1));
    ui->lineEditX2->setText(QString::number(x2));
    ui->lineEditY1->setText(QString::number(y1));
    ui->lineEditY2->setText(QString::number(y2));

    double dx = x2-x1;
    double dy = y2-y1;
    ui->label_dx->setText(QString::number(dx));
    ui->label_dy->setText(QString::number(dy));
    ui->label_1_dx->setText(QString::number(1/dx));
    ui->label_dy_dx->setText(QString::number(dy/dx));
}

void LScopeMenu::parseFromSetting(LObixTreeIter a_obixIter)
{
}

void LScopeMenu::convertToSetting()
{

}

void LScopeMenu::on_checkBoxCursorEnable_stateChanged(int arg1)
{
    emit sendShowCursor(arg1!=0);
#if 0
    if(arg1==0) {
        ui->lineEditX1->clear();
        ui->lineEditX2->clear();
        ui->lineEditY1->clear();
        ui->lineEditY2->clear();

        ui->label_dx->clear();
        ui->label_dy->clear();
        ui->label_1_dx->clear();
        ui->label_dy_dx->clear();
    }
#endif
}

void LScopeMenu::on_checkBoxShowGrid_stateChanged(int arg1)
{
    emit sendShowGrid(arg1!=0);
}
