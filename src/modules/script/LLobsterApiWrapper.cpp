#include "LLobsterApiWrapper.h"

LLobsterApiWrapper::LLobsterApiWrapper(QObject *parent) :
    QObject(parent)
{
    m_pMainApi = new LMainApi;
    m_listApis.append(m_pMainApi);

    m_pPointApi = new LPointApi;
    m_listApis.append(m_pPointApi);

    m_pSystemApi = new LSystemApi;
    m_listApis.append(m_pSystemApi);
}

LLobsterApiWrapper::~LLobsterApiWrapper()
{
    QListIterator<LApi*> it(m_listApis);
    while (it.hasNext()) {
        LApi* pLApi = it.next();
        if (pLApi) {
            delete pLApi;
        }
    }
    m_listApis.clear();
}

QList<LApi *> LLobsterApiWrapper::getApiList() const
{
    return m_listApis;
}

LPointApi *LLobsterApiWrapper::getPointApiObj()
{
    return m_pPointApi;
}

LMainApi *LLobsterApiWrapper::getMainApiObj()
{
    return m_pMainApi;
}

LSystemApi *LLobsterApiWrapper::getSystemApiObj()
{
    return m_pSystemApi;
}
