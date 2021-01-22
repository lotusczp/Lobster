#include "LWatchWindow.h"
#include "ui_LWatchWindow.h"

#include <QCheckBox>
#include <QLabel>
#include <QAction>
#include <QDebug>

#include "LColorSelectButton.h"
#include "LEnumsBox.h"

// define real time value refresh stuff
#define REFRESH_INTERVAL_MS     300

// define column stuff
#define COLUMN_NAME             0
#define COLUMN_VALUE            1
#define COLUMN_ATTRIBUTES       2
#define COLUMN_CURVE_COLOR      3
#define COLUMN_SCOPE_CHECKBOX   4
#define COLUMN_LOG_CHECKBOX     5
#define COLUMN_NUMBER           6

LWatchWindow::LWatchWindow(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LWatchWindow)
{
    ui->setupUi(this);

    m_strRegisterName = "WatchWindow";

    // Configure table widget
    m_uRowCount = 0;
    ui->tableWidget->setRowCount(m_uRowCount);
    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &LWatchWindow::handleCustomContextMenu);

    ui->tableWidget->resizeColumnToContents(COLUMN_SCOPE_CHECKBOX);
    ui->tableWidget->resizeColumnToContents(COLUMN_LOG_CHECKBOX);

    // refresh timer
    m_pRefreshTimer = new QTimer(this);
    m_pRefreshTimer->setInterval(REFRESH_INTERVAL_MS);
    connect(m_pRefreshTimer, &QTimer::timeout, this, &LWatchWindow::refreshValue);

    m_pNameContextMenu = new QMenu(this);
    QAction *pMergeAction = new QAction("Merge Value Axes", m_pNameContextMenu);
    connect(pMergeAction, &QAction::triggered, this, &LWatchWindow::handleMergeValueAxis);
    m_pNameContextMenu->addAction(pMergeAction);
    QAction *pDemergeAction = new QAction("Independent Value Axes", m_pNameContextMenu);
    connect(pDemergeAction, &QAction::triggered, this, &LWatchWindow::handleDemergeValueAxis);
    m_pNameContextMenu->addAction(pDemergeAction);
}

LWatchWindow::~LWatchWindow()
{
    // delete the refresh timer
    if (m_pRefreshTimer != NULL) {
        delete m_pRefreshTimer, m_pRefreshTimer = NULL;
    }

    // delete all items
    QMapIterator<QString, LWatchWindowItem*> i(m_mapItems);
    while(i.hasNext()) {
        i.next();
        if(i.value() != NULL) {
            delete i.value();
        }
    }
    m_mapItems.clear();

    // delete all timeout
    QMapIterator<QString, Timeout*> j(m_pTimeoutMap);
    while(j.hasNext()) {
        j.next();
        if(j.value() != NULL)
            delete j.value();
    }
    m_pTimeoutMap.clear();

    clearHistoryItems();

    delete m_pNameContextMenu;

    delete ui;
}

void LWatchWindow::updateValue(const QString& a_rDataName, const LDataValue& a_rDataValue)
{
    // update value
    if(m_mapItems.contains(a_rDataName)) {
        if (m_mapItems[a_rDataName] != NULL) {
            // a not available data has arrived
            if (a_rDataValue.getCommStatus() == LDataValue::eNormal) {
                if (a_rDataValue.getStatus() == LDataValue::eNotAvailable) {
                    // a new "different" value has arrived
                    if (m_mapItems[a_rDataName]->getValue() != "NA") {
                        // restart time counting
                        if(m_pTimeoutMap.contains(a_rDataName)) {
                            m_pTimeoutMap[a_rDataName]->reset();
                        }
                    }
                    m_mapItems[a_rDataName]->setValue("NA");
                }
                else {
                    // a new "different" value has arrived
                    if (QString::number(a_rDataValue.getValue()) != m_mapItems[a_rDataName]->getValue()) {
                        // restart time counting
                        if(m_pTimeoutMap.contains(a_rDataName)) {
                            m_pTimeoutMap[a_rDataName]->reset();
                        }
                        m_mapItems[a_rDataName]->setValue(QString::number(a_rDataValue.getValue()));
                        // update enums box if necessary
                        updateEnumsBox(a_rDataName, (int)a_rDataValue.getValue());
                    }
                }
            }
            else if (a_rDataValue.getCommStatus() == LDataValue::eCommLoss) {
                m_mapItems[a_rDataName]->setValue("CL");
            }
        }
    }
}

void LWatchWindow::start()
{
    m_pRefreshTimer->start();
}

void LWatchWindow::stop()
{
    m_pRefreshTimer->stop();

    // clear the background color
    QBrush whiteBush(Qt::white);
    for(unsigned int row = 1; row < m_uRowCount; row++) {
        ui->tableWidget->blockSignals(true);
        ui->tableWidget->item(row, COLUMN_VALUE)->setBackground(whiteBush);
        ui->tableWidget->blockSignals(false);
    }
}

void LWatchWindow::updateAttributes(const QString &a_rDataName, const LAttributes &a_rAttributes)
{
    if(m_mapItems.contains(a_rDataName)) {
        // find this row
        QList<QTableWidgetItem *> listItems = ui->tableWidget->findItems(a_rDataName, Qt::MatchCaseSensitive);
        if(!listItems.isEmpty()) {
            int iRow = listItems.first()->row();
            // If enum is available, use enum
            if(a_rAttributes.getEnumsSize() != 0) {
                // Add combo box
                LEnumsBox* pEnumsBox = new LEnumsBox;
                ui->tableWidget->setCellWidget(iRow, COLUMN_ATTRIBUTES, pEnumsBox); // The old widget will be deleted
                QMapIterator<int, QString> it(a_rAttributes.getEnums());
                while(it.hasNext()) {
                    it.next();
                    pEnumsBox->addItem(it.value(), it.key());
                }
                // Set the corresponding value if legal
                QString strValueString = ui->tableWidget->item(iRow, COLUMN_VALUE)->text();
                if(strValueString != "NA") {
                    int iIndex = pEnumsBox->findData(strValueString.toInt());
                    pEnumsBox->setCurrentIndex(iIndex);
                }
                connect(pEnumsBox, SIGNAL(currentIndexChanged(int)), this, SLOT(receiveEnumChanged(int)));
            }
            else {
                // Otherwise just show unit
                QLabel* pUnitLabel = new QLabel(a_rAttributes.getUnit());
                ui->tableWidget->setCellWidget(iRow, COLUMN_ATTRIBUTES, pUnitLabel);
            }
#if 0
            // Mark the value column based on wriable
            if(a_rAttributes.writable()) {
                QTableWidgetItem *pNameItem = ui->tableWidget->item(iRow, COLUMN_NAME);
                QTableWidgetItem *pValueItem = ui->tableWidget->item(iRow, COLUMN_VALUE);
                pValueItem->setFlags(pValueItem->flags() | Qt::ItemIsEditable); // Value Editable
                QBrush whiteBush(Qt::white);
                ui->tableWidget->blockSignals(true);
                pNameItem->setBackground(whiteBush);
                ui->tableWidget->blockSignals(false);
            }
            else {
                QTableWidgetItem *pNameItem = ui->tableWidget->item(iRow, COLUMN_NAME);
                QTableWidgetItem *pValueItem = ui->tableWidget->item(iRow, COLUMN_VALUE);
                pValueItem->setFlags(pValueItem->flags() ^ Qt::ItemIsEditable); // Value not editable
                QBrush grayBush(Qt::gray);
                ui->tableWidget->blockSignals(true);
                pNameItem->setBackground(grayBush);
                ui->tableWidget->blockSignals(false);
            }
#endif
        }
    }
}

void LWatchWindow::parseFromSetting(LObixTreeIter a_obixIter)
{
    clearHistoryItems();

    a_obixIter.moveToRoot();
    a_obixIter.moveToFirstChild();

    if (a_obixIter.getValue().getType() == eObj)  {
        QString strDataName = a_obixIter.getValue().getProperty("is");
        if (strDataName != "") {
            m_mapHistoryItems[strDataName] = new LWatchWindowItem(strDataName);
            a_obixIter.moveToFirstChild();
            if( (a_obixIter.getValue().getType() == eObj) && (a_obixIter.getValue().getProperty("name") == "scope enable")) {
                m_mapHistoryItems[strDataName]->setScopeEnable(a_obixIter.getValue().getProperty("val")=="true" ? true : false);
            }
            a_obixIter.moveToNextSibling();
            if( (a_obixIter.getValue().getType() == eObj) && (a_obixIter.getValue().getProperty("name") == "curve color")) {
                QString strColor = a_obixIter.getValue().getProperty("val");
                QColor color;
                bool ok;
                color.setRed(strColor.mid(1,2).toInt(&ok,16));
                color.setGreen(strColor.mid(3,2).toInt(&ok,16));
                color.setBlue(strColor.mid(5,2).toInt(&ok,16));
                m_mapHistoryItems[strDataName]->setCurveColor(color);
            }
            a_obixIter.moveToNextSibling();
            if( (a_obixIter.getValue().getType() == eObj) && (a_obixIter.getValue().getProperty("name") == "log enable")) {
                m_mapHistoryItems[strDataName]->setLogEnable(a_obixIter.getValue().getProperty("val")=="true" ? true : false);
            }
            a_obixIter.moveToParent();
        }
    }
    while(a_obixIter.hasSibling()) {
        a_obixIter.moveToNextSibling();
        if (a_obixIter.getValue().getType() == eObj)  {
            QString strDataName = a_obixIter.getValue().getProperty("is");
            if (strDataName != "") {
                m_mapHistoryItems[strDataName] = new LWatchWindowItem(strDataName);
                a_obixIter.moveToFirstChild();
                if( (a_obixIter.getValue().getType() == eObj) && (a_obixIter.getValue().getProperty("name") == "scope enable")) {
                    m_mapHistoryItems[strDataName]->setScopeEnable(a_obixIter.getValue().getProperty("val")=="true" ? true : false);
                }
                a_obixIter.moveToNextSibling();
                if( (a_obixIter.getValue().getType() == eObj) && (a_obixIter.getValue().getProperty("name") == "curve color")) {
                    QString strColor = a_obixIter.getValue().getProperty("val");
                    QColor color;
                    bool ok;
                    color.setRed(strColor.mid(1,2).toInt(&ok,16));
                    color.setGreen(strColor.mid(3,2).toInt(&ok,16));
                    color.setBlue(strColor.mid(5,2).toInt(&ok,16));
                    m_mapHistoryItems[strDataName]->setCurveColor(color);
                }
                a_obixIter.moveToNextSibling();
                if( (a_obixIter.getValue().getType() == eObj) && (a_obixIter.getValue().getProperty("name") == "log enable")) {
                    m_mapHistoryItems[strDataName]->setLogEnable(a_obixIter.getValue().getProperty("val")=="true" ? true : false);
                }
                a_obixIter.moveToParent();
            }
        }
    }

}

void LWatchWindow::convertToSetting()
{
    m_obixMutableIter.moveToRoot();
    // build a new subtree
    if (m_obixMutableIter.hasChild())
        m_obixMutableIter.removeChildren();

    QMapIterator<QString, LWatchWindowItem*> i(m_mapItems);
    while(i.hasNext()) {
        i.next();
        if (i.value() != NULL) {
            LObixObj obixObjName("obj", "is", i.key());
            m_obixMutableIter.addChild(obixObjName);

            LObixObj obixObjScopeEnable("obj", "name", "scope enable");
            obixObjScopeEnable.addProperty("val", (i.value()->isScopeEnable() ? "true" : "false"));
            m_obixMutableIter.appendChild(obixObjScopeEnable);

            LObixObj obixObjCurveColor("obj", "name", "curve color");
            QColor color = i.value()->getCurveColor();
            obixObjCurveColor.addProperty("val",
                                          "#" + QString(color.red() < 16? "0" : "") + QString::number(color.red(),16)
                                          + QString(color.green() < 16? "0" : "") + QString::number(color.green(),16)
                                          + QString(color.blue() < 16? "0" : "") + QString::number(color.blue(),16) );
            m_obixMutableIter.appendChild(obixObjCurveColor);

            LObixObj obixObjLogEnable("obj", "name", "log enable");
            obixObjLogEnable.addProperty("val", (i.value()->isLogEnable() ? "true" : "false"));
            m_obixMutableIter.appendChild(obixObjLogEnable);

            m_obixMutableIter.moveToParent();
        }
    }
}

void LWatchWindow::setDataList(const QList<QString> &a_rNameList)
{
    m_listTmpNames.clear();
    m_listTmpNames = m_mapItems.keys();

    QListIterator<QString> i(a_rNameList);
    while (i.hasNext()) {
        QString strDataName = i.next();

        if(!m_mapItems.contains(strDataName)) {
            m_mapItems[strDataName] = new LWatchWindowItem(strDataName);
            // if has this data's history, copy it
            if(m_mapHistoryItems.contains(strDataName)) {
                *m_mapItems[strDataName] = *m_mapHistoryItems[strDataName];
            }

            // add a new line to the table widget
            addItemLine(*m_mapItems[strDataName]);
        }
        else {
            // remove the selected ones, those left in this list are about to be deleted
            m_listTmpNames.removeOne(strDataName);
        }
    }

    // delete the old ones which we do not need any more
    QListIterator<QString> j(m_listTmpNames);
    while (j.hasNext()) {
        QString strDataName = j.next();
        if (m_mapItems.contains(strDataName)) {
            deleteItemLine(strDataName);
            if(m_mapItems[strDataName] != NULL) {
                delete m_mapItems[strDataName];
            }
            m_mapItems.remove(strDataName);
        }
    }

    // delete the history
    clearHistoryItems();

    // Resize the column size
    ui->tableWidget->resizeColumnToContents(COLUMN_NAME);
}

void LWatchWindow::receiveScopeEnableStateByItem(QString name, bool state)
{
     if (m_mapItems.contains(name)) {
         emit sendScopeEnableState(name, state);

         if (state) {
             emit sendCurveColor(name, m_mapItems[name]->getCurveColor());
         }
     }
}

void LWatchWindow::receiveCurveColorChangedByItem(QString name)
{
    if ((m_mapItems.contains(name)) && (m_mapItems[name])) {
        if(m_mapItems[name]->isScopeEnable()) {
            emit sendCurveColor(name, m_mapItems[name]->getCurveColor());
        }
    }
}

void LWatchWindow::receiveLogEnableStateByItem(QString name, bool state)
{
    if (m_mapItems.contains(name)) {
        emit sendLogEnableState(name, state);
    }
}

void LWatchWindow::receiveScopeEnableState(QString name, bool state)
{
    QList<QTableWidgetItem *>  listItems = ui->tableWidget->findItems(name, Qt::MatchCaseSensitive);
    if (!listItems.isEmpty()) {
        int iRow = listItems.first()->row();
        QCheckBox* pScopeCheckBox = qobject_cast<QCheckBox*>(ui->tableWidget->cellWidget(iRow, COLUMN_SCOPE_CHECKBOX));
        if (pScopeCheckBox) {
            pScopeCheckBox->setChecked(state);
        }
    }
}

void LWatchWindow::receiveCurveColor(QString name, QColor curveColor)
{
    QList<QTableWidgetItem *>  listItems = ui->tableWidget->findItems(name, Qt::MatchCaseSensitive);
    if (!listItems.isEmpty()) {
        int iRow = listItems.first()->row();
        LColorSelectButton* pColorSelectButton = qobject_cast<LColorSelectButton*>(ui->tableWidget->cellWidget(iRow, COLUMN_CURVE_COLOR));
        if (pColorSelectButton) {
            pColorSelectButton->setColor(curveColor);
            if (m_mapItems.contains(name)) {
                // set the item color
                m_mapItems[name]->setCurveColor(curveColor);
                // this signal should emit manually because the watchwindow doesn't check it
                emit sendCurveColor(name, curveColor);
            }
        }
    }
}

void LWatchWindow::receiveLogEnableState(QString name, bool state)
{
    QList<QTableWidgetItem *>  listItems = ui->tableWidget->findItems(name, Qt::MatchCaseSensitive);
    if (!listItems.isEmpty()) {
        int iRow = listItems.first()->row();
        QCheckBox* pLogCheckBox = qobject_cast<QCheckBox*>(ui->tableWidget->cellWidget(iRow, COLUMN_LOG_CHECKBOX));
        if (pLogCheckBox) {
            pLogCheckBox->setChecked(state);
        }
    }
}

void LWatchWindow::addItemLine(const LWatchWindowItem &a_rItem)
{
    ui->tableWidget->setSortingEnabled(false);

    // connect signal and slot first
    connect(&a_rItem, &LWatchWindowItem::sendScopeEnableStateByItem, this, &LWatchWindow::receiveScopeEnableStateByItem);
    connect(&a_rItem, &LWatchWindowItem::sendLogEnableStateByItem, this, &LWatchWindow::receiveLogEnableStateByItem);
    connect(&a_rItem, &LWatchWindowItem::sendCurveColorChangedByItem, this, &LWatchWindow::receiveCurveColorChangedByItem);

    // add row count
    m_uRowCount++;
    ui->tableWidget->setRowCount(m_uRowCount);

    // add name
    QTableWidgetItem* pItemName = new QTableWidgetItem(a_rItem.getName());
    ui->tableWidget->setItem(m_uRowCount-1, COLUMN_NAME, pItemName);
    pItemName->setFlags(pItemName->flags() ^ Qt::ItemIsEditable);

    // add value
    QTableWidgetItem* pItemValue = new QTableWidgetItem(a_rItem.getValue());
    ui->tableWidget->blockSignals(true);
    ui->tableWidget->setItem(m_uRowCount-1, COLUMN_VALUE, pItemValue);
    ui->tableWidget->blockSignals(false);

    // add curve color selector
    LColorSelectButton* pColorSelectButton = new LColorSelectButton(this);
    ui->tableWidget->setCellWidget(m_uRowCount-1, COLUMN_CURVE_COLOR, pColorSelectButton);
    connect(pColorSelectButton, &LColorSelectButton::colorChanged, &a_rItem, &LWatchWindowItem::getColorChanged);
    pColorSelectButton->setColor(a_rItem.getCurveColor());

    // add scope checkbox
    QCheckBox* pScopeCheckBox = new QCheckBox(this);
    ui->tableWidget->setCellWidget(m_uRowCount-1, COLUMN_SCOPE_CHECKBOX, pScopeCheckBox);
    connect(pScopeCheckBox, &QCheckBox::stateChanged, &a_rItem, &LWatchWindowItem::getScopeCheckState);
    pScopeCheckBox->setChecked(a_rItem.isScopeEnable());

    // add log checkbox
    QCheckBox* pLogCheckBox = new QCheckBox(this);
    ui->tableWidget->setCellWidget(m_uRowCount-1, COLUMN_LOG_CHECKBOX, pLogCheckBox);
    connect(pLogCheckBox, &QCheckBox::stateChanged, &a_rItem, &LWatchWindowItem::getLogCheckState);
    pLogCheckBox->setChecked(a_rItem.isLogEnable());

    // add a corresponding time out
    if (!m_pTimeoutMap.contains(a_rItem.getName())) {
        m_pTimeoutMap[a_rItem.getName()] = new Timeout(7);
    }
}

void LWatchWindow::deleteItemLine(const QString &a_rItemName)
{   
    // find this row
    int deleteRow = ui->tableWidget->findItems(a_rItemName, Qt::MatchCaseSensitive).first()->row();

    // unckeck all targets
    QCheckBox* pScopeCheckBox = qobject_cast<QCheckBox*>(ui->tableWidget->cellWidget(deleteRow, COLUMN_SCOPE_CHECKBOX));
    if (pScopeCheckBox) {
        pScopeCheckBox->setChecked(false);
    }
    QCheckBox* pLogCheckBox = qobject_cast<QCheckBox*>(ui->tableWidget->cellWidget(deleteRow, COLUMN_LOG_CHECKBOX));
    if (pLogCheckBox) {
        pLogCheckBox->setChecked(false);
    }

    // remove all items of this row
    ui->tableWidget->removeRow(deleteRow);

    // decrease row number
    m_uRowCount--;
    ui->tableWidget->setRowCount(m_uRowCount);

    // delete the corresponding time out
    if (m_pTimeoutMap.contains(a_rItemName)) {
        delete m_pTimeoutMap[a_rItemName];
        m_pTimeoutMap.remove(a_rItemName);
    }
}

void LWatchWindow::clearHistoryItems()
{
    // delete all items
    QMapIterator<QString, LWatchWindowItem*> i(m_mapHistoryItems);
    while(i.hasNext()) {
        i.next();
        if(i.value() != NULL) {
            delete i.value();
        }
    }
    m_mapHistoryItems.clear();
}

void LWatchWindow::updateEnumsBox(const QString &a_rstrName, int a_iValue)
{
    if (m_mapItems.contains(a_rstrName)) {
        int iRow = ui->tableWidget->findItems(a_rstrName, Qt::MatchCaseSensitive).first()->row();
        LEnumsBox* pEnumsBox = qobject_cast<LEnumsBox*>(ui->tableWidget->cellWidget(iRow, COLUMN_ATTRIBUTES));
        if(pEnumsBox) { // There is a check box
            int iIndex = pEnumsBox->findData(a_iValue);
            if(iIndex != -1) {
                pEnumsBox->blockSignals(true); // Don't send index change signal
                pEnumsBox->setCurrentIndex(iIndex);
                pEnumsBox->blockSignals(false);
            }
        }
    }
}

void LWatchWindow::refreshValue()
{
    // refresh value
    /// \note notice that the first row is changed to 0 since version 2.3
    for(unsigned int row = 0; row < m_uRowCount; row++) {
        QString strDataName = ui->tableWidget->item(row, COLUMN_NAME)->text();
        if (m_mapItems.contains(strDataName)) {
            QString strValue = m_mapItems[strDataName]->getValue();
            ui->tableWidget->blockSignals(true);
            ui->tableWidget->item(row, COLUMN_VALUE)->setText(strValue);
            ui->tableWidget->blockSignals(false);
        }
    }

    // change background color
    QMapIterator<QString, Timeout*> i(m_pTimeoutMap);
    while(i.hasNext()) {
        i.next();
        i.value()->tick();
        if(i.value()->isTimeout()) {
            QBrush whiteBush(Qt::white);
            int iRow = ui->tableWidget->findItems(i.key(), Qt::MatchCaseSensitive).first()->row();
            ui->tableWidget->blockSignals(true);
            ui->tableWidget->item(iRow, COLUMN_VALUE)->setBackground(whiteBush);
            ui->tableWidget->blockSignals(false);
        }
        else {
            QBrush yellowBush(Qt::yellow);
            int iRow = ui->tableWidget->findItems(i.key(), Qt::MatchCaseSensitive).first()->row();
            ui->tableWidget->blockSignals(true);
            ui->tableWidget->item(iRow, COLUMN_VALUE)->setBackground(yellowBush);
            ui->tableWidget->blockSignals(false);
        }
    }
}

void LWatchWindow::receiveEnumChanged(int index)
{
    LEnumsBox* pEnumsBox = qobject_cast<LEnumsBox*>(this->sender());
    for(unsigned int row = 0; row < m_uRowCount; row++) {
        if(pEnumsBox == ui->tableWidget->cellWidget(row, COLUMN_ATTRIBUTES)) {
            // There must be a match
            QVariant value = pEnumsBox->itemData(index);
            if(value.type() == QVariant::Int) {
                ui->tableWidget->item(row, COLUMN_VALUE)->setText(QString::number(value.toInt()));
            }
            break;
        }
    }
}

void LWatchWindow::handleCustomContextMenu(const QPoint &pos)
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

void LWatchWindow::handleMergeValueAxis()
{
    QList<QString> listNames;
    QList<QTableWidgetItem *> selectedItems = ui->tableWidget->selectedItems();
    if(selectedItems.size() > 1) {
        foreach(QTableWidgetItem * pItem, selectedItems) {
            if(pItem && pItem->column() == COLUMN_NAME) {
                listNames.append(pItem->text());
            }
        }
        emit sendMergeValueAxis(listNames);
    }
}

void LWatchWindow::handleDemergeValueAxis()
{
    QList<QString> listNames;
    QList<QTableWidgetItem *> selectedItems = ui->tableWidget->selectedItems();
    if(selectedItems.size() > 1) {
        foreach(QTableWidgetItem * pItem, selectedItems) {
            if(pItem && pItem->column() == COLUMN_NAME) {
                listNames.append(pItem->text());
            }
        }
        emit sendDemergeValueAxis(listNames);
    }
}

void LWatchWindow::on_tableWidget_cellChanged(int row, int column)
{
    if(column == COLUMN_VALUE) {
        QString strDataName = ui->tableWidget->item(row, COLUMN_NAME)->text();
        if (m_mapItems.contains(strDataName)) {
            QString strValueText = ui->tableWidget->item(row, COLUMN_VALUE)->text();
            bool ok;
            double dValue = strValueText.toDouble(&ok);
            if (ok) {
                // a valid number
                LDataValue dataValue;
                dataValue.set(dValue);
                emit sendDataValue(strDataName, dataValue);
            }
            else if (strValueText.toUpper() == "NA") {
                // imply that this data is not available
                LDataValue dataValue;
                dataValue.setStatus(LDataValue::eNotAvailable);
                // send the signal
                emit sendDataValue(strDataName, dataValue);
            }
            else if (strValueText.toUpper() == "CL") {
                // imply that this data need to disconnect
                LDataValue dataValue;
                dataValue.setCommStatus(LDataValue::eCommLoss);
                // send the signal
                emit sendDataValue(strDataName, dataValue);
            }
        }
    }
}

void LWatchWindow::on_tableWidget_cellClicked(int row, int column)
{
    if(ui->tableWidget->selectedItems().size() == 1 // Single selection
            && column == COLUMN_NAME) {
        if (m_strSelectedName != ui->tableWidget->item(row, COLUMN_NAME)->text()) {
            // set unselected curve to normal weight
            if ( m_mapItems.contains(m_strSelectedName) ) {
                // send the signal
                emit sendCurveSelected(m_strSelectedName, false);
            }
        }
        m_strSelectedName = ui->tableWidget->item(row, COLUMN_NAME)->text();
        // set curve thicker
        if ( m_mapItems.contains(m_strSelectedName) ) {
            // send the signal
            emit sendCurveSelected(m_strSelectedName, true);
        }
    }
    else {
        // hasn't select a name
        // set unselected curve to normal weight
        if ( m_mapItems.contains(m_strSelectedName) ) {
            // send the signal
            emit sendCurveSelected(m_strSelectedName, false);
        }
    }
}

void LWatchWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    if(column == COLUMN_NAME) {
        QString strSelectedName = ui->tableWidget->item(row, COLUMN_NAME)->text();
        // Pull the value
        if ( m_mapItems.contains(strSelectedName) ) {
            emit sendPullValueCmd(strSelectedName);
        }
    }
}
