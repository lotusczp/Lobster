#include "LSaveLog.h"
#include "ui_LSaveLog.h"

#include <QMessageBox>
#include <QFileDialog>
#include "LTdmFileHandler.h"

#define ROWS_SCAN_INTERVAL  100

#define COLUMN_NAME         0
#define COLUMN_VALUE        1
#define COLUMN_ABS_TIME     2

LSaveLog::LSaveLog(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LSaveLog)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->scrollToBottom();

    // refresh timer
    m_pScanTimer = new QTimer(this);
    m_pScanTimer->setInterval(ROWS_SCAN_INTERVAL);
    connect(m_pScanTimer, &QTimer::timeout, this, &LSaveLog::scanRows);
    m_pScanTimer->start();

    // build a new one
    m_pDataLogHandler = new LTdmFileHandler();

    // init current row number
    m_uCurrentRowNumber = 0;

    m_bIsObserverStarted = false;
    m_bIsSaving = false;

    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
}

LSaveLog::~LSaveLog()
{
    m_pScanTimer->stop();

    m_pDataLogHandler->closeFile();

    if (m_pDataLogHandler != NULL) {
        delete m_pDataLogHandler;
    }
    if (m_pScanTimer != NULL) {
        delete m_pScanTimer;
    }
    delete ui;
}

void LSaveLog::updateValue(const QString& a_rDataName, const LDataValue& a_rDataValue)
{
    if(m_bIsObserverStarted) {
        QString strValue = QString::number(a_rDataValue.getValue());
        QString strAbsTime = QString::number(a_rDataValue.getAbsTime());

        // Saving log file if needed
        if(m_bIsSaving) {
            QStringList rowStringList;
            rowStringList << a_rDataName;
            rowStringList << strValue;
            rowStringList << strAbsTime;
            m_pDataLogHandler->addNewLine(rowStringList);
        }

        //Inserts an empty row into the table after current row
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        m_uCurrentRowNumber++;

        QTableWidgetItem* pItemName = new QTableWidgetItem(a_rDataName);
        ui->tableWidget->setItem(m_uCurrentRowNumber-1,COLUMN_NAME,pItemName);

        QTableWidgetItem* pItemValue = a_rDataValue.getStatus()==LDataValue::eAvailable ?
                                        new QTableWidgetItem(strValue) :
                                        new QTableWidgetItem("NA");
        ui->tableWidget->setItem(m_uCurrentRowNumber-1,COLUMN_VALUE,pItemValue);

        QTableWidgetItem* pItemAbsTime = new QTableWidgetItem(strAbsTime);
        ui->tableWidget->setItem(m_uCurrentRowNumber-1,COLUMN_ABS_TIME,pItemAbsTime);
    }
}

void LSaveLog::start()
{
    m_bIsObserverStarted = true;
}

void LSaveLog::stop()
{
    m_bIsObserverStarted = false;
}

void LSaveLog::scanRows()
{
    if (ui->tableWidget->rowCount() > 100) {
        // Clear the view if too many rows
        m_uCurrentRowNumber = 0;
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);
    }
}

void LSaveLog::on_pushButtonStart_clicked()
{
    if (!m_pDataLogHandler->openFile()) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"),"Some errors occur when trying to write log file");
        return;
    }

    m_bIsSaving = true;
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);
}

void LSaveLog::on_pushButtonStop_clicked()
{
    if (!m_pDataLogHandler->saveFile()) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"),"Some errors occur when trying to save log file");
        return;
    }

    if (!m_pDataLogHandler->closeFile()) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"),"Some errors occur when trying to close log file");
        return;
    }

    m_bIsSaving = false;
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
}
