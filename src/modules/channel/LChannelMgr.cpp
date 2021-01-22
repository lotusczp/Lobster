#include "LChannelMgr.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QMessageBox>

#include "LDataManager.h"
#include "LErrorHandler.h"


LChannelMgr* LChannelMgr::s_pInstance = NULL;


void openWarningBox(const QString &a_rWarningMsg)
{
    QMessageBox::warning(0,QObject::tr("Warning"),a_rWarningMsg);
}


LChannelMgr *LChannelMgr::instance()
{
    // Do we have an oracle yet?
    if(s_pInstance == NULL) {
      // No, create it
      s_pInstance = new LChannelMgr();
    }

    // Return the instance ptr
    return s_pInstance;
}

void LChannelMgr::destroy()
{
    // Clean up the pointer
    if(s_pInstance != NULL) {
        delete s_pInstance, s_pInstance = NULL;
    }
}

void LChannelMgr::addChannel(const QString &a_rChannelName, const QString &a_rCfgPath, LTransmission& a_rTransmission)
{
    m_mapChannels[a_rChannelName] = new LChannel(a_rChannelName, a_rCfgPath, a_rTransmission);
}

void LChannelMgr::deleteChannel(const QString &a_rChannelName)
{
    if (m_mapChannels.contains(a_rChannelName)) {

        if (m_mapChannels[a_rChannelName] != NULL) {
            delete m_mapChannels[a_rChannelName], m_mapChannels[a_rChannelName] = NULL;
        }

        m_mapChannels.remove(a_rChannelName);
    }
}

QString LChannelMgr::getCfgPathByChannel(const QString &a_rChannelName) const
{
    QString strPath;
    if (m_mapChannels.contains(a_rChannelName)) {
        strPath = m_mapChannels[a_rChannelName]->getCfgPath();
    }
    return strPath;
}

void LChannelMgr::setCfgPathByChannel(const QString &a_rChannelName, const QString &a_rConfigPath)
{
    if (m_mapChannels.contains(a_rChannelName)) {
        m_mapChannels[a_rChannelName]->setCfgPath(a_rConfigPath);
    }
}

void LChannelMgr::activateChannel(const QString &a_rChannelName)
{
    if (m_mapChannels.contains(a_rChannelName))
    {
        m_mapChannels[a_rChannelName]->setActive(true);

        if(m_mapChannels[a_rChannelName]->getTransmissionPtr() != NULL) {

            qRegisterMetaType<LDataValueList>("LDataValueList");
            qRegisterMetaType<LDataValueMap>("LDataValueMap");

            /// \note Disconnect first, if already connected, the slot will be called twice
            disconnect(m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::notifySingleVar, LDataManager::instance(), &LDataManager::getNotifySingleVar);
            connect(m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::notifySingleVar, LDataManager::instance(), &LDataManager::getNotifySingleVar);

            disconnect(m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::notifyMultiVar, LDataManager::instance(), &LDataManager::getNotifyMultiVar);
            connect(m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::notifyMultiVar, LDataManager::instance(), &LDataManager::getNotifyMultiVar);

            disconnect(m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::errorInfo, LErrorHandler::instance(), &LErrorHandler::handlePluginError);
            connect(m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::errorInfo, LErrorHandler::instance(), &LErrorHandler::handlePluginError);

            disconnect(m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::notifyVarAttributes, LDataManager::instance(), &LDataManager::getNotifyVarAttributes);
            connect(m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::notifyVarAttributes, LDataManager::instance(), &LDataManager::getNotifyVarAttributes);

            // send value from lobster
            disconnect(LDataManager::instance(), &LDataManager::sendDataValue, m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::setDataValue);
            connect(LDataManager::instance(), &LDataManager::sendDataValue, m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::setDataValue);

            // pull value by lobster
            disconnect(LDataManager::instance(), &LDataManager::pullDataValue, m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::pullDataValue);
            connect(LDataManager::instance(), &LDataManager::pullDataValue, m_mapChannels[a_rChannelName]->getTransmissionPtr(), &LTransmission::pullDataValue);
        }
    }
}

void LChannelMgr::deactivateChannel(const QString &a_rChannelName)
{
    if (m_mapChannels.contains(a_rChannelName))
    {
        m_mapChannels[a_rChannelName]->setActive(false);
    }
}

bool LChannelMgr::isChannelActive(const QString &a_rChannelName) const
{
    return m_mapChannels[a_rChannelName]->isActive();
}

void LChannelMgr::startActiveChannels()
{
    QMapIterator<QString, LChannel*> i(m_mapChannels);
    while (i.hasNext())
    {
        i.next();
        if (i.value() != NULL)
        {
            if ((i.value()->isActive()) && (i.value()->getTransmissionPtr() != NULL)) {
                i.value()->startTransmission();
            }
        }
    }
}

void LChannelMgr::stopActiveChannels()
{
    QMapIterator<QString, LChannel*> i(m_mapChannels);
    while (i.hasNext())
    {
        i.next();
        if (i.value() != NULL)
        {
            if ((i.value()->isActive()) && (i.value()->getTransmissionPtr() != NULL)) {
                i.value()->stopTransmission();
            }
        }
    }
}

void LChannelMgr::configurateAllChannels()
{
    QMapIterator<QString, LChannel*> i(m_mapChannels);
    while (i.hasNext())
    {
        i.next();
        if (i.value() != NULL)
        {
            configChannel(i.key(), i.value()->getCfgPath());
        }
    }
}

QList<QString> LChannelMgr::getChannelList() const
{
    return m_mapChannels.keys();
}

void LChannelMgr::setTransmissionPtr(const QString &a_rChannelName, LTransmission& a_rTransmission)
{
    if ((m_mapChannels.contains(a_rChannelName)) && (m_mapChannels[a_rChannelName] != NULL ) ) {
        m_mapChannels[a_rChannelName]->setTransmissionPtr(a_rTransmission);
    }
}

void LChannelMgr::resetCaredDataList(const QString &a_rChannelName, const QList<QString> &a_rCaredDataList)
{
    if ((m_mapChannels.contains(a_rChannelName)) && (m_mapChannels[a_rChannelName] != NULL ) ) {
        m_mapChannels[a_rChannelName]->resetCaredDataList(a_rCaredDataList);
    }
}

bool LChannelMgr::getDataListByChannel(const QString &a_rChannelName, QList<QString> &a_rDataList) const
{
    if ((m_mapChannels.contains(a_rChannelName)) && (m_mapChannels[a_rChannelName] != NULL ) ) {
        return m_mapChannels[a_rChannelName]->getDataList(a_rDataList);
    }
    else {
        return false;
    }
}

void LChannelMgr::getAvailableDatas(QList<QString> &a_rWholeDataList) const
{
    QList<QString> listAllDatas;
    QMapIterator<QString, LChannel*> i(m_mapChannels);
    while (i.hasNext()) {
        i.next();
        QList<QString> listData;
        if (i.value()->getDataList(listData) ) {
            // add a prefix of channel name
            QListIterator<QString> j(listData);
            while(j.hasNext()) {
                QString strDataName = j.next();
                listAllDatas.append(i.key()+"_"+strDataName);
            }
        }
        else {
            openWarningBox("The Channel " + i.key() +" has some problems parsing the configuation file. \nPlease check it.");
        }
    }
    a_rWholeDataList = listAllDatas;
}

QWidget *LChannelMgr::getSettingsWidget(const QString &a_rChannelName)
{
    if ((m_mapChannels.contains(a_rChannelName)) && (m_mapChannels[a_rChannelName] != NULL ) ) {
        return m_mapChannels[a_rChannelName]->getSettingsWidget();
    }
    else {
        return NULL;
    }
}

QWidget *LChannelMgr::getToolWidget(const QString &a_rChannelName)
{
    if ((m_mapChannels.contains(a_rChannelName)) && (m_mapChannels[a_rChannelName] != NULL ) ) {
        return m_mapChannels[a_rChannelName]->getToolWidget();
    }
    else {
        return NULL;
    }
}

void LChannelMgr::pullAttriFromAllChannels()
{
    QMapIterator<QString, LChannel*> i(m_mapChannels);
    while (i.hasNext()) {
        i.next();
        if (i.value() != NULL) {
            i.value()->pullAttributes();
        }
    }
}

LChannelMgr::LChannelMgr()
{
    LObixObj obixObj("list", "is", "All channels");
    m_pObixTree = new LTree<LObixObj>(obixObj);
}

LChannelMgr::~LChannelMgr()
{
    clearChannelMap();

    if (m_pObixTree != NULL) {
        delete m_pObixTree, m_pObixTree = NULL;
    }
}

void LChannelMgr::clearChannelMap()
{
    QMapIterator<QString, LChannel*> i(m_mapChannels);
    while (i.hasNext())
    {
        i.next();
        if (i.value() != NULL)
        {
            delete i.value();
        }
    }
    m_mapChannels.clear();
}

void LChannelMgr::configChannel(const QString &a_rChannelName, const QString &a_rFilename)
{
    #define START_ELEMENT   0
    #define END_ELEMENT     1
    unsigned int lastElement = START_ELEMENT;

    // step 1 : parse the xml file to a tree
    QFile file(a_rFilename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QXmlStreamReader reader(&file);

        LTree<LObixObj>::Iterator it(*m_pObixTree);
        it.moveToRoot();

        LObixObj obixObj("obj", "is", a_rChannelName);
        it.addChild(obixObj);


        while (!reader.atEnd())
        {
            if(reader.isStartElement()) {
                LObixObj obixObjTmp;
                obixObjTmp.setType(reader.name().toString());
                if(!reader.attributes().isEmpty()) {
                    QVectorIterator<QXmlStreamAttribute> iter(reader.attributes());
                    while(iter.hasNext()) {
                        QXmlStreamAttribute attribute = iter.next();
                        obixObjTmp.addProperty(attribute.name().toString(), attribute.value().toString());
                    }
                }
                if (lastElement == START_ELEMENT) {
                    it.addChild(obixObjTmp);
                }
                else if(lastElement == END_ELEMENT) {
                    while(it.hasSibling()) {
                        it.moveToNextSibling();
                    }
                    it.insertSibling(obixObjTmp);
                }
                lastElement = START_ELEMENT;
            }
            else if(reader.isEndElement()) {
                if (lastElement == END_ELEMENT) {
                    it.moveToParent();
                }
                lastElement = END_ELEMENT;
            }

            reader.readNext();
        }
        file.close();

        // step 2 : find the subtree of this channel
        it.moveToRoot();
        if (it.hasChild()) {
            it.moveToFirstChild();
            while(it.getValue().getProperty("is") != a_rChannelName) {
                it.moveToNextSibling();
            }
            it.moveToFirstChild();

            // step 3: config this channel
            if(m_mapChannels.contains(a_rChannelName)) {
                m_mapChannels[a_rChannelName]->configChannel(m_pObixTree->constSubTree(it));
            }
        }
    }
    else
    {
        openWarningBox("Can not open " + a_rFilename + ".\nRead configuration failed.\nPlease check whether this file exists.");
    }
}
