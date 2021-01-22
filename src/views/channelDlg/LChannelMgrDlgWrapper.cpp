#include "LChannelMgrDlgWrapper.h"


LChannelMgrDlgWrapper::LChannelMgrDlgWrapper()
{
    m_strRegisterName = "ChannelMgr";
}

LChannelMgrDlgWrapper::~LChannelMgrDlgWrapper()
{
}

void LChannelMgrDlgWrapper::loadHistory()
{
    m_pDialog->loadHistory(m_mapChannelContent);
}

void LChannelMgrDlgWrapper::createDialog(QWidget *parent)
{
    m_pDialog = new LChannelMgrDlg(parent);
    m_pDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_pDialog, &LChannelMgrDlg::acceptChannel, this, &LChannelMgrDlgWrapper::getAcceptSig);
}

void LChannelMgrDlgWrapper::execDialog()
{
    m_pDialog->exec();
}

void LChannelMgrDlgWrapper::clickOk()
{
    m_pDialog->clickOk();
}

void LChannelMgrDlgWrapper::getAcceptSig(QMap<QString, ChannelContent> a_mapChannelContent)
{
    // send names of deleted channels
    LDeletedChannelList listChannelNames;
    ChlMapIter i(m_mapChannelContent);
    while (i.hasNext()) {
        i.next();
        if (!a_mapChannelContent.contains(i.key())) {
            // this channel has been deleted
            listChannelNames.append(i.key());
        }
    }
    emit sendDeletedChannels(listChannelNames);

    // new channel map
    m_mapChannelContent = a_mapChannelContent;

    convertToSetting();

    if (m_pDialog != NULL) {
        delete m_pDialog, m_pDialog = NULL;
    }
}

void LChannelMgrDlgWrapper::parseFromSetting(LObixTreeIter a_obixIter)
{
    m_mapChannelContent.clear();

    a_obixIter.moveToRoot();
    if(a_obixIter.hasChild()) {
        a_obixIter.moveToFirstChild();
        if( (a_obixIter.getValue().getType() == eObj) && (a_obixIter.getValue().getProperty("is") != "") ) {
            QString strChannelName = a_obixIter.getValue().getProperty("is");
            QString strConfigPath = "";
            QString strDllPath = "";
            a_obixIter.moveToFirstChild();
            if ( (a_obixIter.getValue().getType() == eStr) && (a_obixIter.getValue().getProperty("name") == "Configuration Path") ) {
                strConfigPath = a_obixIter.getValue().getProperty("val");
            }
            a_obixIter.moveToNextSibling();
            if ( (a_obixIter.getValue().getType() == eStr) && (a_obixIter.getValue().getProperty("name") == "Plugin Path") ) {
                strDllPath = a_obixIter.getValue().getProperty("val");
            }
            if( (strConfigPath != "") && (strDllPath != "")) {
                m_mapChannelContent[strChannelName].name = strChannelName;
                m_mapChannelContent[strChannelName].configPath = strConfigPath;
                m_mapChannelContent[strChannelName].pluginPath = strDllPath;
            }

            a_obixIter.moveToParent();
        }
        while (a_obixIter.hasSibling()) {
            a_obixIter.moveToNextSibling();

            if( (a_obixIter.getValue().getType() == eObj) && (a_obixIter.getValue().getProperty("is") != "") ) {
                QString strChannelName = a_obixIter.getValue().getProperty("is");
                QString strConfigPath = "";
                QString strDllPath = "";
                a_obixIter.moveToFirstChild();
                if ( (a_obixIter.getValue().getType() == eStr) && (a_obixIter.getValue().getProperty("name") == "Configuration Path") ) {
                    strConfigPath = a_obixIter.getValue().getProperty("val");
                }
                a_obixIter.moveToNextSibling();
                if ( (a_obixIter.getValue().getType() == eStr) && (a_obixIter.getValue().getProperty("name") == "Plugin Path") ) {
                    strDllPath = a_obixIter.getValue().getProperty("val");
                }
                if( (strConfigPath != "") && (strDllPath != "")) {
                    m_mapChannelContent[strChannelName].name = strChannelName;
                    m_mapChannelContent[strChannelName].configPath = strConfigPath;
                    m_mapChannelContent[strChannelName].pluginPath = strDllPath;
                }

                a_obixIter.moveToParent();

            }
        }
    }

}

void LChannelMgrDlgWrapper::convertToSetting()
{
    m_obixMutableIter.moveToRoot();
    // build a new subtree
    if (m_obixMutableIter.hasChild())
        m_obixMutableIter.removeChildren();
    QListIterator<ChannelContent> i(m_mapChannelContent.values());
    while (i.hasNext()) {
        ChannelContent channelContent = i.next();

        LObixObj obixObjName("obj", "is", channelContent.name);
        m_obixMutableIter.addChild(obixObjName);

        LObixObj obixObjConfigPath("str", "name", "Configuration Path");
        obixObjConfigPath.addProperty("val", channelContent.configPath);
        m_obixMutableIter.appendChild(obixObjConfigPath);

        LObixObj obixObjDllPath("str", "name", "Plugin Path");
        obixObjDllPath.addProperty("val", channelContent.pluginPath);
        m_obixMutableIter.appendChild(obixObjDllPath);

        m_obixMutableIter.moveToRoot();
    }
}

