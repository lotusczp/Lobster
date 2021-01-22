#ifndef LLOGSPINBOX_H
#define LLOGSPINBOX_H

#include <QDoubleSpinBox>

class LLogSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit LLogSpinBox(QWidget *parent = 0);

    void setNum(double value);

signals:

public slots:

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent * event);

    void increaseNum();

    void decreaseNum();

    double m_dOutputValue;
};

#endif // LLOGSPINBOX_H
