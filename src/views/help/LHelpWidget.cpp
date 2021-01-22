#include "LHelpWidget.h"
#include "ui_LHelpWidget.h"
#include "LUniqueResource.h"

LHelpWidget::LHelpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LHelpWidget)
{
    ui->setupUi(this);

    // set scale
    QList<int> sizeList;
    sizeList.append(100);
    sizeList.append(600);
    ui->splitter->setSizes(sizeList);

    // help content brower
    ui->homeButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "home.png")));
    ui->homeButton->adjustSize();
    ui->backwardButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "backward.png")));
    ui->backwardButton->adjustSize();
    ui->backwardButton->setEnabled(false);
    ui->forwardButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "forward.png")));
    ui->forwardButton->adjustSize();
    ui->forwardButton->setEnabled(false);

    connect(ui->textBrowser, &QTextBrowser::backwardAvailable, ui->backwardButton, &QPushButton::setEnabled);
    connect(ui->textBrowser, &QTextBrowser::forwardAvailable, ui->forwardButton, &QPushButton::setEnabled);

    ui->textBrowser->setSource(QUrl::fromLocalFile(PATH_ACCESSER->getPath(ePathDoc, "index.html")));
}

LHelpWidget::~LHelpWidget()
{
    delete ui;
}

void LHelpWidget::on_homeButton_clicked()
{
    // start all over
    ui->textBrowser->clearHistory();
    ui->backwardButton->setEnabled(false);
    ui->forwardButton->setEnabled(false);
    // set homepage
    ui->textBrowser->setSource(QUrl::fromLocalFile(PATH_ACCESSER->getPath(ePathDoc, "index.html")));
}

void LHelpWidget::on_backwardButton_clicked()
{
    ui->textBrowser->backward();
}

void LHelpWidget::on_forwardButton_clicked()
{
    ui->textBrowser->forward();
}

void LHelpWidget::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    // start all over
    ui->textBrowser->clearHistory();
    ui->backwardButton->setEnabled(false);
    ui->forwardButton->setEnabled(false);
    // set the corresponding page
    ui->textBrowser->setSource(QUrl::fromLocalFile(PATH_ACCESSER->getPath(ePathDoc, item->text() + ".html")));
}
