#ifndef LENUMSBOX_H
#define LENUMSBOX_H

#include <QComboBox>

class LEnumsBox : public QComboBox
{
    Q_OBJECT
public:
    explicit LEnumsBox(QWidget *parent = 0);

protected:
    virtual void wheelEvent(QWheelEvent * e) Q_DECL_OVERRIDE;
};

#endif // LENUMSBOX_H
