#ifndef LCONTROLPAGE_H
#define LCONTROLPAGE_H

#include <QWidget>

namespace Ui {
class LControlPage;
}

class LControlPage : public QWidget
{
    Q_OBJECT

public:
    explicit LControlPage(QWidget *parent = nullptr);
    ~LControlPage();

    void setControlPage(const QString &a_rHtmlPath);

private:
    Ui::LControlPage *ui;
};

#endif // LCONTROLPAGE_H
