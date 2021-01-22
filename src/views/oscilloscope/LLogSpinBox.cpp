#include <QKeyEvent>
#include "cmath"

#include "LLogSpinBox.h"

// low limit should be pow(10,x)
#define LOW_LIMIT 0.1

LLogSpinBox::LLogSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent)
{
    setButtonSymbols(QAbstractSpinBox::NoButtons);
    setSuffix(" s");
    setMinimum(LOW_LIMIT);
    // init it with the min value
    setValue(LOW_LIMIT);
    m_dOutputValue = minimum();
}

void LLogSpinBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up)
    {
        increaseNum();
    }
    else if(event->key() == Qt::Key_Down)
    {
        decreaseNum();
    }

    this->setValue(m_dOutputValue);
}

void LLogSpinBox::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0)
    {
        increaseNum();
    }
    else if (event->delta() < 0)
    {
        decreaseNum();
    }

    this->setValue(m_dOutputValue);
}

void LLogSpinBox::increaseNum()
{
    double dValue = this->value();
    int iPower = (int)floor(log10(dValue));
    int iNumber = (int)(dValue/pow(10.0,iPower));
    switch(iNumber) {
    case 1:
        m_dOutputValue = pow(10.0,iPower) * 2;
        break;
    case 2:
        m_dOutputValue = pow(10.0,iPower) * 5;
        break;
    case 5:
        m_dOutputValue = pow(10.0,iPower+1);
        break;
    default:
        // can not reach here
        break;
    }

    if (m_dOutputValue < minimum()) {
        m_dOutputValue = minimum();
    }
}

void LLogSpinBox::decreaseNum()
{
    double dValue = this->value();
    int iPower = (int)floor(log10(dValue));
    int iNumber = (int)(dValue/pow(10.0,iPower));
    switch(iNumber) {
    case 1:
        m_dOutputValue = pow(10.0,iPower-1) * 5;
        break;
    case 2:
        m_dOutputValue = pow(10.0,iPower);
        break;
    case 5:
        m_dOutputValue = pow(10.0,iPower) * 2;
        break;
    default:
        // can not reach here
        break;
    }

    if (m_dOutputValue < minimum()) {
        m_dOutputValue = minimum();
    }
}

void LLogSpinBox::setNum(double value)
{
    if (value < minimum()) {
        m_dOutputValue = minimum();
    }
    else {
        int iPower = (int)floor(log10(value));
        double dNumber = (value/pow(10.0,iPower));
        if (dNumber!=1 && dNumber!=2 && dNumber!=5) {
            // In case the value is changed manully
            // Normally this will not happen
            if (dNumber < 1) {
                m_dOutputValue = pow(10.0,iPower-1) * 5;
            }
            else if (dNumber < 2) {
                m_dOutputValue = pow(10.0,iPower);
            }
            else if (dNumber < 5) {
                m_dOutputValue = pow(10.0,iPower) * 2;
            }
            else {
                m_dOutputValue = pow(10.0,iPower) * 5;
            }
        }
        else {
            m_dOutputValue = value;
        }
    }
    this->setValue(m_dOutputValue);
}
