#ifndef LSETTINGSTABDIALOG_H
#define LSETTINGSTABDIALOG_H

#include <QDialog>

namespace Ui {
class LSettingsTabDialog;
}

class LSettingsTabDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LSettingsTabDialog(QWidget *parent = 0);
    ~LSettingsTabDialog();

    //! add tab to the tab widget
    int	addTab(QWidget * page, const QString & label);

    //! count of tabs
    int	count() const;

private:
    Ui::LSettingsTabDialog *ui;
};

#endif // LSETTINGSTABDIALOG_H
