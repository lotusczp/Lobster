#ifndef LSCOPEMENU_H
#define LSCOPEMENU_H

#include <QWidget>

#include "LSettingsRelier.h"

namespace Ui {
class LScopeMenu;
}

class LScopeMenu : public QWidget, public LSettingsRelier
{
    Q_OBJECT

public:
    explicit LScopeMenu(QWidget *parent = 0);
    ~LScopeMenu();

    void receiveCursorInfo(double x1, double y1, double x2, double y2);

signals:
    void sendShowCursor(bool a_bShow);

    void sendShowGrid(bool a_bShow);

private slots:
    void on_checkBoxCursorEnable_stateChanged(int arg1);

    void on_checkBoxShowGrid_stateChanged(int arg1);

protected:
    virtual void parseFromSetting(LObixTreeIter a_obixIter) Q_DECL_OVERRIDE;

    virtual void convertToSetting() Q_DECL_OVERRIDE;

private:
    Ui::LScopeMenu *ui;
};

#endif // LSCOPEMENU_H
