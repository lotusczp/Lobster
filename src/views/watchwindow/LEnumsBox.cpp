#include "LEnumsBox.h"

#include <QWheelEvent>

LEnumsBox::LEnumsBox(QWidget *parent) :
    QComboBox(parent)
{

}

void LEnumsBox::wheelEvent(QWheelEvent *e)
{
    // Disable mouse wheel features
    e->ignore();
}
