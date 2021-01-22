#include "LMainSettingsMgr.h"
#include "LUniqueResource.h"

LMainSettingsMgr* LMainSettingsMgr::s_pInstance = NULL;

LMainSettingsMgr *LMainSettingsMgr::instance()
{
    // Do we have an oracle yet?
    if(s_pInstance == NULL) {
      // No, create it
      s_pInstance = new LMainSettingsMgr();
    }

    // Return the instance ptr
    return s_pInstance;
}

void LMainSettingsMgr::destroy()
{
    // Clean up the pointer
    if(s_pInstance != NULL) {
        delete s_pInstance, s_pInstance = NULL;
    }
}

void LMainSettingsMgr::registerToSettingsMgr(LSettingsRelier *a_pSettingsRelier)
{
    m_settingsMgr.registerToSettingsMgr(a_pSettingsRelier);
}

void LMainSettingsMgr::unregisterToSettingsMgr(const QString &a_rstrRegisterName)
{
    m_settingsMgr.unregisterToSettingsMgr(a_rstrRegisterName);
}

void LMainSettingsMgr::readSettingsFile()
{
    m_settingsMgr.readSettingsFile(m_strHistoryFilePath);
}

void LMainSettingsMgr::writeSettingsFile()
{
    m_settingsMgr.writeSettingsFile(m_strHistoryFilePath);
}

LMainSettingsMgr::LMainSettingsMgr()
{
    m_strHistoryFilePath = PATH_ACCESSER->getPath(ePathProject, "HistoryMemory.xml");
}
