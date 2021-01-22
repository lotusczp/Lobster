#include "LColorSelectButton.h"
#include "ui_LColorSelectButton.h"

#include <QColorDialog>
#include <QColor>

LColorSelectButton::LColorSelectButton(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LColorSelectButton)
{
    ui->setupUi(this);
#if 0
    ui->label->setVisible(false);
#endif
}

LColorSelectButton::~LColorSelectButton()
{
    delete ui;
}

void LColorSelectButton::setColor(const QColor &a_rColor)
{
    if (a_rColor != m_color) {
        m_color = a_rColor;
        applyColorChange();
    }
}

const QColor &LColorSelectButton::getColor() const
{
    return m_color;
}

void LColorSelectButton::on_pushButton_clicked()
{
    QColor color = QColorDialog::getColor(m_color, this, "Select Color");
    // if color changed
    if ((color.isValid()) && (color != m_color)) {
        m_color = color;
        applyColorChange();
        emit colorChanged(m_color);
    }
}

void LColorSelectButton::applyColorChange()
{
    QString strColor("#"
                     + QString(m_color.red() < 16? "0" : "") + QString::number(m_color.red(),16)
                     + QString(m_color.green() < 16? "0" : "") + QString::number(m_color.green(),16)
                     + QString(m_color.blue() < 16? "0" : "") + QString::number(m_color.blue(),16) );

    QString styleSheet("background: " + strColor + ";");

    ui->pushButton->setStyleSheet(styleSheet);
    ui->pushButton->update();
#if 0
    ui->label->setText(strColor);
#endif
}

