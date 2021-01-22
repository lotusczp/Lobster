#include "LSelectDataDlg.h"
#include "ui_LSelectDataDlg.h"

#include "LDataManager.h"

LSelectDataDlg::LSelectDataDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LSelectDataDlg)
{
    ui->setupUi(this);
}

LSelectDataDlg::~LSelectDataDlg()
{
    delete ui;
}

void LSelectDataDlg::setDataNameCollection(const QStringList &a_rDataNameCol)
{
    ui->listWidgetCollection->addItems(a_rDataNameCol);
}

void LSelectDataDlg::loadHistory(const QList<QString> &a_rSelectNames)
{
    QListIterator<QString> i(a_rSelectNames);
    while(i.hasNext()) {
        QString strName = i.next();
        QList<QListWidgetItem*> listItem = ui->listWidgetCollection->findItems(strName, Qt::MatchCaseSensitive);
        if (!listItem.isEmpty()) {
            QListWidgetItem* pItem = listItem.first();
            // delete history selection in the collection list
            // and add it to select list
            // same as "double click" this item
            on_listWidgetCollection_itemDoubleClicked(pItem);
        }
    }
}

void LSelectDataDlg::clickOk()
{
    emit ui->buttonBox->accepted();
}

void LSelectDataDlg::selectName(const QString &a_rstrDataName)
{
    QList<QListWidgetItem*> listItem = ui->listWidgetCollection->findItems(a_rstrDataName, Qt::MatchCaseSensitive);
    if (!listItem.isEmpty()) {
        QListWidgetItem* pItem = listItem.first();
        // delete history selection in the collection list
        // and add it to select list
        // same as "double click" this item
        on_listWidgetCollection_itemDoubleClicked(pItem);
    }
}

void LSelectDataDlg::unselectName(const QString &a_rstrDataName)
{
    QList<QListWidgetItem*> listItem = ui->listWidgetSelected->findItems(a_rstrDataName, Qt::MatchCaseSensitive);
    if (!listItem.isEmpty()) {
        QListWidgetItem* pItem = listItem.first();
        // delete history selection in the collection list
        // and add it to select list
        // same as "double click" this item
        on_listWidgetSelected_itemDoubleClicked(pItem);
    }
}

void LSelectDataDlg::on_lineEditKeyWord_textChanged(const QString &arg1)
{
    QList<QListWidgetItem*> listSearchItems = ui->listWidgetCollection->findItems(arg1, Qt::MatchContains);
    for(int row = 0; row < ui->listWidgetCollection->count(); row++) {
        QListWidgetItem* pItem = ui->listWidgetCollection->item(row);
        if( listSearchItems.contains(pItem) ) {
            // this satisfies the search condition
            pItem->setHidden(false);
        }
        else {
            // this does not satisfy the search condition
            pItem->setHidden(true);
        }
    }
}

void LSelectDataDlg::on_lineEditKeyWordSelect_textChanged(const QString &arg1)
{
    QList<QListWidgetItem*> listSearchItems = ui->listWidgetSelected->findItems(arg1, Qt::MatchContains);
    for(int row = 0; row < ui->listWidgetSelected->count(); row++) {
        QListWidgetItem* pItem = ui->listWidgetSelected->item(row);
        if( listSearchItems.contains(pItem) ) {
            // this satisfies the search condition
            pItem->setHidden(false);
        }
        else {
            // this does not satisfy the search condition
            pItem->setHidden(true);
        }
    }
}

void LSelectDataDlg::on_listWidgetCollection_itemDoubleClicked(QListWidgetItem *item)
{
    // add to temp memory
    QString strFullName = item->text();
    QString strProtocol = strFullName.left(strFullName.indexOf("_"));
    //QString strName = strFullName.right(strFullName.length()-1-strFullName.indexOf("_"));
    if ( !m_mapTempStorage.contains(strProtocol) ) {
        // if not created yet
        m_mapTempStorage[strProtocol].append(strFullName);
    }
    else {
        if (!m_mapTempStorage[strProtocol].contains(strFullName)) {
            m_mapTempStorage[strProtocol].append(strFullName);
        }
    }

    // show in the selected list
    ui->listWidgetSelected->addItem(strFullName);

    // delete in the collection list
    ui->listWidgetCollection->removeItemWidget(item);
    delete item, item = NULL;
}

void LSelectDataDlg::on_listWidgetSelected_itemDoubleClicked(QListWidgetItem *item)
{
    // delete the corresponding temp memory
    QString strFullName = item->text();
    QString strProtocol = strFullName.left(strFullName.indexOf("_"));
    //QString strName = strFullName.right(strFullName.length()-1-strFullName.indexOf("_"));
    if ( !m_mapTempStorage.contains(strProtocol) ) {
        // should not get here
    }
    else {
        if (m_mapTempStorage[strProtocol].contains(strFullName)) {
            m_mapTempStorage[strProtocol].removeOne(strFullName);

            // if delete all the data of this protocol
            if (m_mapTempStorage[strProtocol].isEmpty()) {
                LDataManager::instance()->unsubscribeChannel(strProtocol);

                m_mapTempStorage.remove(strProtocol);
            }
        }
    }

    // show in the selected list
    ui->listWidgetCollection->addItem(strFullName);

    // delete in the collection list
    ui->listWidgetSelected->removeItemWidget(item);
    delete item, item = NULL;
}

void LSelectDataDlg::on_buttonBox_accepted()
{
    // subscribe data names to data manager
    LDataManager::instance()->subscribeDataList(m_mapTempStorage);

    QList<QString> listAllSelectedNames;
    QMapIterator<QString, QList<QString> > i(m_mapTempStorage);
    while(i.hasNext()) {
        i.next();
        listAllSelectedNames.append(i.value());
    }
    emit acceptSelection(listAllSelectedNames);
}

void LSelectDataDlg::on_pushButtonAdd_clicked()
{
    QListIterator<QListWidgetItem*> i(ui->listWidgetCollection->selectedItems());
    while (i.hasNext())
    {
        QListWidgetItem* pItem = i.next();
        if (pItem != NULL) {
            on_listWidgetCollection_itemDoubleClicked(pItem);
        }
    }
}

void LSelectDataDlg::on_pushButtonAddAll_clicked()
{
    ui->listWidgetCollection->selectAll();
    on_pushButtonAdd_clicked();
}

void LSelectDataDlg::on_pushButtonRemove_clicked()
{
    QListIterator<QListWidgetItem*> i(ui->listWidgetSelected->selectedItems());
    while (i.hasNext())
    {
        QListWidgetItem* pItem = i.next();
        if (pItem != NULL) {
            on_listWidgetSelected_itemDoubleClicked(pItem);
        }
    }
}

void LSelectDataDlg::on_pushButtonRemoveAll_clicked()
{
    ui->listWidgetSelected->selectAll();
    on_pushButtonRemove_clicked();
}
