#ifndef LCOLORSELECTBUTTON_H
#define LCOLORSELECTBUTTON_H

#include <QWidget>

namespace Ui {
class LColorSelectButton;
}

class LColorSelectButton : public QWidget
{
    Q_OBJECT

public:
    explicit LColorSelectButton(QWidget *parent = 0);
    ~LColorSelectButton();

    void setColor(const QColor &a_rColor);
    const QColor& getColor() const;

signals:
    void colorChanged(QColor color);

private slots:
    void on_pushButton_clicked();

private:
    void applyColorChange();

private:
    Ui::LColorSelectButton *ui;

    QColor m_color; //!< color of the button
};

#endif // LCOLORSELECTBUTTON_H
