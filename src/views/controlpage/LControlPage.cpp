#include "LControlPage.h"
#include "ui_LControlPage.h"

LControlPage::LControlPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LControlPage)
{
    ui->setupUi(this);
}

LControlPage::~LControlPage()
{
    delete ui;
}

void LControlPage::setControlPage(const QString &a_rHtmlPath)
{
    ui->webView->load(QUrl(a_rHtmlPath));
    ui->webView->show();
}
