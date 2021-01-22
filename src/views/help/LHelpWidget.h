#ifndef LHELPWIDGET_H
#define LHELPWIDGET_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class LHelpWidget;
}

class LHelpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LHelpWidget(QWidget *parent = 0);
    ~LHelpWidget();

private slots:
    void on_homeButton_clicked();

    void on_backwardButton_clicked();

    void on_forwardButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::LHelpWidget *ui;
};

#endif // LHELPWIDGET_H
