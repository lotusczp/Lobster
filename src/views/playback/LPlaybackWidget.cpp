#include "LPlaybackWidget.h"
#include "ui_LPlaybackWidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include "LColorSelectButton.h"

// define column stuff
#define COLUMN_NAME             0
#define COLUMN_CURVE_COLOR      1

#define DEFAULT_COLOR_NUM       12
static const QColor defaultColorArray[DEFAULT_COLOR_NUM] = {
    QColor("#ff3D3D"),
    QColor("#23AE23"),
    QColor("#F53093"),
    QColor("#3C48E8"),
    QColor("#FFFF80"),
    QColor("#FF9595"),
    QColor("#FFCA95"),
    QColor("#AFFF95"),
    QColor("#D5FFF4"),
    QColor("#95CAFF"),
    QColor("#9595FF"),
    QColor("#FF95E4"),
};

static quint32 calcHash(const QString& str)
{
    quint32 hash = 0;
    if (str.length() == 0) return hash;
    for (int i=0, len=str.length(); i<len; i++) {
        hash  = hash + str.at(i).unicode();
    }
    return hash;
}


LPlaybackWidget::LPlaybackWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LPlaybackWidget)
{
    ui->setupUi(this);

    ui->tableWidget->setSortingEnabled(false);

    ui->scope->setFeatures(QDockWidget::NoDockWidgetFeatures);

    m_uRowCount = 0;

    m_pParserThread = new QThread;
    m_logFileParser.moveToThread(m_pParserThread);
    connect(this, &LPlaybackWidget::sendFileToParse, &m_logFileParser, &LLogFileParser::parseFile);
    connect(&m_logFileParser, &LLogFileParser::sendDataValue, this, &LPlaybackWidget::receiveDataValue);
    connect(&m_logFileParser, &LLogFileParser::sendErrors, this, &LPlaybackWidget::receiveErrors);
    connect(&m_logFileParser, &LLogFileParser::sendNames, this, &LPlaybackWidget::receiveNames);
    connect(&m_logFileParser, &LLogFileParser::sendProcessFinished, this, &LPlaybackWidget::receiveProcessFinished);
    connect(&m_logFileParser, &LLogFileParser::sendProcessPercent, this, &LPlaybackWidget::receiveProcessPercent);


    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &LPlaybackWidget::handleCustomContextMenu);
    m_pNameContextMenu = new QMenu(this);
    QAction *pMergeAction = new QAction("Merge Value Axes", m_pNameContextMenu);
    connect(pMergeAction, &QAction::triggered, this, &LPlaybackWidget::handleMergeValueAxis);
    m_pNameContextMenu->addAction(pMergeAction);
    QAction *pDemergeAction = new QAction("Independent Value Axes", m_pNameContextMenu);
    connect(pDemergeAction, &QAction::triggered, this, &LPlaybackWidget::handleDemergeValueAxis);
    m_pNameContextMenu->addAction(pDemergeAction);

    m_pParserThread->start();
}

LPlaybackWidget::~LPlaybackWidget()
{
    m_pParserThread->quit();
    m_pParserThread->wait();
    delete m_pParserThread;

    delete ui;
}

void LPlaybackWidget::receiveCurveColorChangedByItem(QString name)
{
    if ((m_mapItems.contains(name)) && (m_mapItems[name])) {
        ui->scope->setCurveColor(name, m_mapItems[name]->getCurveColor());
    }
}

void LPlaybackWidget::receiveDataValue(QString name, LDataValue value)
{
    ui->scope->updateValue(name, value);
}

void LPlaybackWidget::receiveErrors(QString errorMessage)
{
    QMessageBox::warning(0, QObject::tr("Warning"), errorMessage);
}

void LPlaybackWidget::receiveNames(QStringList valueNames)
{
    qDeleteAll(m_mapItems);
    m_mapItems.clear();
    m_uRowCount = 0;
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(valueNames.size());

    foreach (QString name, valueNames) {
        ui->scope->addNewCurve(name);
        QColor color = defaultColorArray[calcHash(name)%DEFAULT_COLOR_NUM];
        ui->scope->setCurveColor(name, color);

        // Add to table widget
        LPlaybackWidgetTableItem* pItem = new LPlaybackWidgetTableItem(name);
        if(!m_mapItems.contains(name)) {
            m_mapItems.insert(name, pItem);
        }
        pItem->setCurveColor(color);
        connect(pItem, &LPlaybackWidgetTableItem::sendCurveColorChangedByItem, this, &LPlaybackWidget::receiveCurveColorChangedByItem);

        // Add name
        QTableWidgetItem* pItemName = new QTableWidgetItem(pItem->getName());
        ui->tableWidget->setItem(m_uRowCount, COLUMN_NAME, pItemName);
        pItemName->setFlags(pItemName->flags() ^ Qt::ItemIsEditable);

        // add curve color selector
        LColorSelectButton* pColorSelectButton = new LColorSelectButton(this);
        ui->tableWidget->setCellWidget(m_uRowCount, COLUMN_CURVE_COLOR, pColorSelectButton);
        connect(pColorSelectButton, &LColorSelectButton::colorChanged, pItem, &LPlaybackWidgetTableItem::getColorChanged);
        pColorSelectButton->setColor(pItem->getCurveColor());

        m_uRowCount++;
    }

    // Resize the column size
    ui->tableWidget->resizeColumnToContents(COLUMN_NAME);
}

void LPlaybackWidget::receiveProcessFinished()
{
    ui->progressBar->setValue(100); // Force 100% if not
    ui->scope->stop();
}

void LPlaybackWidget::receiveProcessPercent(int percent)
{
    ui->progressBar->setValue(percent);
}

void LPlaybackWidget::handleCustomContextMenu(const QPoint &pos)
{
    if(ui->tableWidget->itemAt(pos) != nullptr
            && ui->tableWidget->itemAt(pos)->column() == COLUMN_NAME) {
        QList<QTableWidgetItem *> selectedItems = ui->tableWidget->selectedItems();
        if(selectedItems.size() > 1) {
            bool allSelectedAreNames = true;
            foreach(QTableWidgetItem * pItem, selectedItems) {
                if(pItem && pItem->column() != COLUMN_NAME) {
                    allSelectedAreNames = false;
                    break;
                }
            }

            if(allSelectedAreNames) {
                m_pNameContextMenu->move(cursor().pos());
                m_pNameContextMenu->show();
            }
        }
    }
}

void LPlaybackWidget::handleMergeValueAxis()
{
    QList<QString> listNames;
    QList<QTableWidgetItem *> selectedItems = ui->tableWidget->selectedItems();
    if(selectedItems.size() > 1) {
        foreach(QTableWidgetItem * pItem, selectedItems) {
            if(pItem && pItem->column() == COLUMN_NAME) {
                listNames.append(pItem->text());
            }
        }
        ui->scope->mergeValueAxis(listNames);
    }
}

void LPlaybackWidget::handleDemergeValueAxis()
{
    QList<QString> listNames;
    QList<QTableWidgetItem *> selectedItems = ui->tableWidget->selectedItems();
    if(selectedItems.size() > 1) {
        foreach(QTableWidgetItem * pItem, selectedItems) {
            if(pItem && pItem->column() == COLUMN_NAME) {
                listNames.append(pItem->text());
            }
        }
        ui->scope->demergeValueAxis(listNames);
    }
}

void LPlaybackWidget::on_browseButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Open Log file"),
                                                    tr("./datalog/"),
                                                    tr("Log file (*.tdm)"));
    ui->filePathEdit->setText(filePath);
}

void LPlaybackWidget::on_loadButton_clicked()
{
    // Clear first
    ui->scope->start();

    emit sendFileToParse(ui->filePathEdit->text());
}

void LPlaybackWidget::on_tableWidget_cellClicked(int row, int column)
{
    if(ui->tableWidget->selectedItems().size() == 1 // Single selection
            && column == COLUMN_NAME) {
        if (m_strSelectedName != ui->tableWidget->item(row, COLUMN_NAME)->text()) {
            // set unselected curve to normal weight
            if ( m_mapItems.contains(m_strSelectedName) ) {
                // send the signal
                ui->scope->setCurveSelected(m_strSelectedName, false);
            }
        }
        m_strSelectedName = ui->tableWidget->item(row, COLUMN_NAME)->text();
        // set curve thicker
        if ( m_mapItems.contains(m_strSelectedName) ) {
            // send the signal
            ui->scope->setCurveSelected(m_strSelectedName, true);
        }
    }
    else {
        // hasn't select a name
        // set unselected curve to normal weight
        if ( m_mapItems.contains(m_strSelectedName) ) {
            // send the signal
            ui->scope->setCurveSelected(m_strSelectedName, false);
        }
    }
}
