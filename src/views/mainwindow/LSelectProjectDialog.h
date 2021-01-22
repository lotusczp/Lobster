#ifndef LSELECTPROJECTDIALOG_H
#define LSELECTPROJECTDIALOG_H

#include <QDialog>

namespace Ui {
class LSelectProjectDialog;
}

class LSelectProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LSelectProjectDialog(QWidget *parent = nullptr);
    ~LSelectProjectDialog();

    QString getProjectPath() const;

private slots:
    void on_browseButton_clicked();

private:
    Ui::LSelectProjectDialog *ui;
};

#endif // LSELECTPROJECTDIALOG_H
