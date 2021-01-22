// lobster headers
#include "LDataManager.h"
#include "LChannelMgr.h"


LDataManager* LDataManager::s_pInstance = NULL;


LDataManager::LDataManager() :
    m_bIsRunning(false)
{
}

LDataManager::~LDataManager()
{
    QMapIterator<QString, LData*> i(m_mapDataList);
    while(i.hasNext()) {
        i.next();
        if (i.value() != NULL) {
            delete i.value();
        }
    }
    m_mapDataList.clear();
}

LDataManager* LDataManager::instance()
{
    // Do we have an oracle yet?
    if(s_pInstance == NULL) {
      // No, create it
      s_pInstance = new LDataManager();
    }

    // Return the instance ptr
    return s_pInstance;
}

void LDataManager::destroy()
{
    // Clean up the pointer
    if(s_pInstance != NULL) {
        delete s_pInstance, s_pInstance = NULL;
    }
}

void LDataManager::startActiveChannels()
{
    m_bIsRunning = true;
    LChannelMgr::instance()->startActiveChannels();
}

void LDataManager::pauseActiveChannels()
{
    m_bIsRunning = false;
}

void LDataManager::stopActiveChannels()
{
    LChannelMgr::instance()->stopActiveChannels();
}

void LDataManager::fetchAllAvailableDataNames()
{
    // clear all the names;
    m_listDataNameCollections.clear();
    
    // get new names
    LChannelMgr::instance()->getAvailableDatas(m_listDataNameCollections);
}

const QStringList &LDataManager::getDataNameCollection() const
{
    return m_listDataNameCollections;
}

void LDataManager::subscribeDataList(const QMap<QString, QList<QString> > &a_rDataList)
{
    m_listExistingNames.clear();
    m_listExistingNames = m_mapDataList.keys();

    QMapIterator<QString, QList<QString> > i(a_rDataList);
    while (i.hasNext()) {
        i.next();
        subscribeDataByChannel(i.key(), i.value());
    }

    // delete the old ones which we do not need any more
    QListIterator<QString> j(m_listExistingNames);
    while (j.hasNext()) {
        QString strDataName = j.next();
        if (m_mapDataList.contains(strDataName)) {
            delete m_mapDataList[strDataName], m_mapDataList[strDataName] = NULL;
            m_mapDataList.remove(strDataName);
        }
    }
}

void LDataManager::subscribeDataByChannel(const QString &a_rChannelName, const QList<QString> &a_rNamesWithPrefix)
{
    QListIterator<QString> i(a_rNamesWithPrefix);

    QList<QString> listCaredNames;

    while (i.hasNext()) {
        QString strNameWithPrefix = i.next();
        // step 1: get the name without channel prefix for the transmission
        QString strNameWithoutPrefix = strNameWithPrefix.right(strNameWithPrefix.length()-1-strNameWithPrefix.indexOf("_"));

        if (!m_mapDataList.contains(strNameWithPrefix) ) {
            // step 2: create data instance if not created yet
            m_mapDataList[strNameWithPrefix] = new LData(strNameWithPrefix);
        }
        else {
            // step 3: cross out the data in the temp memory of the list selected last time, prepare for deleting the unselected datas
            m_listExistingNames.removeOne(strNameWithPrefix);
        }

        listCaredNames.append(strNameWithoutPrefix);
    }

    // step 4: register to corresponding channnel manager
    LChannelMgr::instance()->resetCaredDataList(a_rChannelName, listCaredNames);
    LChannelMgr::instance()->activateChannel(a_rChannelName);

}

void LDataManager::unsubscribeChannel(const QString &a_rChannelName)
{
    LChannelMgr::instance()->deactivateChannel(a_rChannelName);
}

void LDataManager::attachValueObserver(const QString &a_rDataName, LValueObserver *a_pObserver)
{
    if (m_mapDataList.contains(a_rDataName)) {
        m_mapDataList[a_rDataName]->attachValueObserver(a_pObserver);
    }
}

void LDataManager::detachValueObserver(const QString &a_rDataName, LValueObserver *a_pObserver)
{
    if (m_mapDataList.contains(a_rDataName)) {
        m_mapDataList[a_rDataName]->detachValueObserver(a_pObserver);
    }
}

void LDataManager::attachAttriObserver(const QString &a_rDataName, LAttributesObserver *a_pObserver)
{
    if (m_mapDataList.contains(a_rDataName)) {
        m_mapDataList[a_rDataName]->attachAttriObserver(a_pObserver);
    }
}

void LDataManager::detachAttriObserver(const QString &a_rDataName, LAttributesObserver *a_pObserver)
{
    if (m_mapDataList.contains(a_rDataName)) {
        m_mapDataList[a_rDataName]->detachAttriObserver(a_pObserver);
    }
}

void LDataManager::getNotifySingleVar(QString a_strDataName, LDataValueList a_lstDataValue)
{
    // if the pause the transmission, the datas will be sent to data manager but will not be processed
    if (m_bIsRunning) {
        if (m_mapDataList.contains(a_strDataName)) {
            // push value to buffer
            QListIterator<LDataValue> i(a_lstDataValue);
            while (i.hasNext()) {
                LDataValue value = i.next();
                m_mapDataList[a_strDataName]->pushValue(value);
            }

            // notify all observers
            m_mapDataList[a_strDataName]->notifyAllValueObs();
        }
    }
}

void LDataManager::getNotifyMultiVar(LDataValueMap a_mapDataValue)
{
    // if the pause the transmission, the datas will be sent to data manager but will not be processed
    if (m_bIsRunning) {
        QMapIterator<QString, LDataValue> i(a_mapDataValue);
        while (i.hasNext()) {
            i.next();
            if (m_mapDataList.contains(i.key())) {
                // push value to buffer
                m_mapDataList[i.key()]->pushValue(i.value());
                // notify all observers
                m_mapDataList[i.key()]->notifyAllValueObs();
            }
        }
    }
}

void LDataManager::receiveDataValue(QString a_strDataName, LDataValue a_dDataValue)
{
    emit sendDataValue(a_strDataName, a_dDataValue);
}

void LDataManager::receivePullValueCmd(QString a_strDataName)
{
    emit pullDataValue(a_strDataName);
}

void LDataManager::getNotifyVarAttributes(LAttributesMap a_mapAttributes)
{
    /// \note Even not running, this should work because the attibutes can
    ///       be updated before running.
    QMapIterator<QString, LAttributes> i(a_mapAttributes);
    while (i.hasNext()) {
        i.next();
        if (m_mapDataList.contains(i.key())) {
            m_mapDataList[i.key()]->setAttributes(i.value());
            // notify all observers
            m_mapDataList[i.key()]->notifyAllAttriObs();
        }
    }
}
