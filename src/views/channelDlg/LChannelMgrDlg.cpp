#include "LChannelMgrDlg.h"
#include "ui_LChannelMgrDlg.h"

#include <QPluginLoader>
#include <QMessageBox>

#include "LAddNewChannelDlg.h"
#include "LChannelMgr.h"
#include "LPluginSettingsMgr.h"
#include "LTransPluginFactory.h"
#include "LUniqueResource.h"


LChannelMgrDlg::LChannelMgrDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LChannelMgrDlg)
{
    ui->setupUi(this);

    ui->deleteChannel->setEnabled(false);
    ui->reloadChannel->setEnabled(false);

    ui->lineEditName->setEnabled(false);
    ui->lineEditConfigPath->setEnabled(false);
    ui->lineEditPluginPath->setEnabled(false);
}

LChannelMgrDlg::~LChannelMgrDlg()
{
    delete ui;
}

void LChannelMgrDlg::loadHistory(const QMap<QString, ChannelContent> &a_rMapChannelContent)
{
    m_mapChannelContent = a_rMapChannelContent;

    ui->listChannelNames->addItems(m_mapChannelContent.keys());
}

void LChannelMgrDlg::clickOk()
{
    emit ui->buttonBox->accepted();
}

void LChannelMgrDlg::getNewChannel(QString a_strName, QString a_strConfigPath, QString a_strPluginPath)
{
    if (m_mapChannelContent.contains(a_strName)) {
        QMessageBox::warning(0, QObject::tr("Warning"), "The Channel has already existed!\nThe Add opration discard.\nIf you want to edit it you may use line Editor below");
    }
    else {
        m_mapChannelContent[a_strName].name = a_strName;
        m_mapChannelContent[a_strName].pluginPath = a_strPluginPath;
        m_mapChannelContent[a_strName].configPath = a_strConfigPath;

        refreshChannelList();
    }
}

void LChannelMgrDlg::on_addChannel_clicked()
{
    LAddNewChannelDlg *dialog = new LAddNewChannelDlg(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &LAddNewChannelDlg::sendNewChannel, this, &LChannelMgrDlg::getNewChannel);
    dialog->exec();
}

void LChannelMgrDlg::on_deleteChannel_clicked()
{
    /// \note list widget should be single selection only
    QListWidgetItem *item = ui->listChannelNames->selectedItems().first();

    if (item != NULL) {
        if (m_mapChannelContent.contains(item->text())) {
            m_mapChannelContent.remove(item->text());
        }
    }

    // refresh
    refreshChannelList();
}

void LChannelMgrDlg::on_buttonBox_accepted()
{
    ChannelContentMap mapNewlyAdded;
    QList<QString> listOldChannels = LChannelMgr::instance()->getChannelList();

    // create new added map
    ChlMapIter i(m_mapChannelContent);
    while (i.hasNext()) {
        i.next();
        if (!listOldChannels.contains(i.key())) {
            mapNewlyAdded[i.key()] = i.value();
        }
    }
    // delete unselected channels
    QListIterator<QString> j(listOldChannels);
    while (j.hasNext()) {
        QString strChannelName = j.next();
        if (!m_mapChannelContent.contains(strChannelName)) {
            deleteChannel(strChannelName);
        }
    }

    // send information to wapper
    emit acceptChannel(m_mapChannelContent);

    applyChannelContent(mapNewlyAdded);
}

void LChannelMgrDlg::refreshChannelList()
{
    ui->listChannelNames->clear();

    ChlMapIter i(m_mapChannelContent);
    while(i.hasNext()) {
        i.next();
        ui->listChannelNames->addItem(i.key());
    }
}

bool LChannelMgrDlg::applyChannelContent(const ChannelContentMap& a_rmapChannelContent)
{
    bool res = false;

    ChlMapIter channelContentIter(a_rmapChannelContent);
    while (channelContentIter.hasNext()) {
        channelContentIter.next();
        QPluginLoader loader(PATH_ACCESSER->getPath(SubPath::ePathRoot, channelContentIter.value().pluginPath));
        QObject *pInstance = loader.instance();
        if(pInstance) {
            LTransPluginFactory *pTransFactory = qobject_cast<LTransPluginFactory*>(pInstance);
            if(pTransFactory) {
                LTransmission *pTransmission = pTransFactory->createTransInstance();
                delete pTransFactory, pTransFactory = NULL;
                if(pTransmission) {
                    LChannelMgr::instance()->addChannel(channelContentIter.key(),
                                                        PATH_ACCESSER->getPath(ePathProject, channelContentIter.value().configPath),
                                                        *pTransmission);
                    // Set channel prefix
                    pTransmission->setChannelPrefix(channelContentIter.key());
                    // Handle settings relier
                    if (pTransmission->getSettingsRelier() != NULL) {
                        // set channel name as the register name
                        pTransmission->getSettingsRelier()->setRegisterName(channelContentIter.key());
                        // register to the manager
                        LPluginSettingsMgr::instance()->registerToSettingsMgr(pTransmission->getSettingsRelier());
                    }
                    // Set unique resource
                    pTransmission->setUniqueResource(LUniqueResourceMgr::instance()->getUniqueResource());
                    // Return value
                    res = true;
                }
                else {
                    QMessageBox::warning(0, QObject::tr("Warning"), "Can not parse this plugin: \n" + channelContentIter.value().pluginPath
                                         + "\nSome plugin can NOT be loaded twice. Please check it");
                    res = false;
                }
            }
            else {
                QMessageBox::warning(0, QObject::tr("Warning"), "Bad implementation of plugin file: \n" + channelContentIter.value().pluginPath);
                res = false;
            }
        }
        else {
            QMessageBox::warning(0, QObject::tr("Warning"), channelContentIter.value().pluginPath + "\nLoad plugin file failed, Error Message is as below:\n" + loader.errorString());
            res = false;
        }
    }

    // configurate all channel
    LChannelMgr::instance()->configurateAllChannels();

    return res;
}

void LChannelMgrDlg::deleteChannel(const QString &a_rstrChannelName)
{
    // the channel name is the register name
    /// \note if this channel do not need settings relier, this function will do nothing
    LPluginSettingsMgr::instance()->unregisterToSettingsMgr(a_rstrChannelName);

    // delete this channel transmission instance
    if (LChannelMgr::instance()->getChannelList().contains(a_rstrChannelName)) {
        LChannelMgr::instance()->deleteChannel(a_rstrChannelName);
    }
}

void LChannelMgrDlg::on_listChannelNames_itemClicked(QListWidgetItem *item)
{
    // enable delete
    ui->deleteChannel->setEnabled(true);
    // enable reload button
    ui->reloadChannel->setEnabled(true);

    // enable edit
    QString strChannelName = item->text();

    ui->lineEditName->setEnabled(true);
    ui->lineEditConfigPath->setEnabled(true);
    ui->lineEditPluginPath->setEnabled(true);

    ui->lineEditName->setText(strChannelName);
    ui->lineEditConfigPath->setText(m_mapChannelContent[strChannelName].configPath);
    ui->lineEditPluginPath->setText(m_mapChannelContent[strChannelName].pluginPath);
}

void LChannelMgrDlg::on_listChannelNames_itemSelectionChanged()
{
    // none is selected
    if (ui->listChannelNames->selectedItems().isEmpty()) {
        // disable delete button
        ui->deleteChannel->setEnabled(false);
        // disable reload button
        ui->reloadChannel->setEnabled(false);
        // erase line editor
        ui->lineEditName->clear();
        ui->lineEditName->setEnabled(false);

        ui->lineEditConfigPath->clear();
        ui->lineEditConfigPath->setEnabled(false);

        ui->lineEditPluginPath->clear();
        ui->lineEditPluginPath->setEnabled(false);
    }

}

void LChannelMgrDlg::on_lineEditName_editingFinished()
{
    if (!ui->listChannelNames->selectedItems().isEmpty()) {
        /// \note list widget should be single selection only
        QString strOriginalName = ui->listChannelNames->selectedItems().first()->text();
        QString strNewName = ui->lineEditName->text();
        if (strNewName == "") {
            QMessageBox::warning(0, QObject::tr("Warning"), "The name can not be empty");
            ui->lineEditName->setText(strOriginalName);
        }
        else {
            // the name already used by another channel
            if ((strNewName != strOriginalName) && (m_mapChannelContent.contains(strNewName))) {
                QMessageBox::warning(0, QObject::tr("Warning"), "Name already used");
                ui->lineEditName->setText(strOriginalName);
            }
            // the name of selected channel has been changed to another
            if (!m_mapChannelContent.contains(strNewName)) {
                on_deleteChannel_clicked();
                m_mapChannelContent[strNewName].name = strNewName;
                m_mapChannelContent[strNewName].configPath = ui->lineEditConfigPath->text();
                m_mapChannelContent[strNewName].pluginPath = ui->lineEditPluginPath->text();

                // refresh
                refreshChannelList();
            }
        }
    }
}

void LChannelMgrDlg::on_lineEditConfigPath_editingFinished()
{
    QString strName = ui->lineEditName->text();
    if (m_mapChannelContent.contains(strName)) {
        m_mapChannelContent[strName].configPath = ui->lineEditConfigPath->text();
    }
}

void LChannelMgrDlg::on_lineEditPluginPath_editingFinished()
{
    QString strName = ui->lineEditName->text();
    if (m_mapChannelContent.contains(strName)) {
        m_mapChannelContent[strName].pluginPath = ui->lineEditPluginPath->text();
    }
}

void LChannelMgrDlg::on_reloadChannel_clicked()
{
    /// \note list widget should be single selection only
    QListWidgetItem *item = ui->listChannelNames->selectedItems().first();

    if (item != NULL) {
        QString strChannelName = item->text();
        if (m_mapChannelContent.contains(strChannelName)) {
            ChannelContentMap reloadContent;
            reloadContent.clear();
            reloadContent[strChannelName] = m_mapChannelContent[strChannelName];

            // step 1 : delete this channel first
            deleteChannel(strChannelName);

            // step 2 : add this channel
            applyChannelContent(reloadContent);
        }
    }
}

void LChannelMgrDlg::on_reloadAllChannels_clicked()
{
    // step 1 : clear all plugin instance(s), waiting for loading new ones
    QListIterator<QString> i(m_mapChannelContent.keys());
    while(i.hasNext()) {
        QString strChannelName = i.next();
        deleteChannel(strChannelName);
    }

    // step 2 : add all channels back
    applyChannelContent(m_mapChannelContent);
}
