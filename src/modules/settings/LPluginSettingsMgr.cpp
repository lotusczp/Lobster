#include "LPluginSettingsMgr.h"
#include "LUniqueResource.h"

LPluginSettingsMgr* LPluginSettingsMgr::s_pInstance = NULL;

LPluginSettingsMgr *LPluginSettingsMgr::instance()
{
    // Do we have an oracle yet?
    if(s_pInstance == NULL) {
      // No, create it
      s_pInstance = new LPluginSettingsMgr();
    }

    // Return the instance ptr
    return s_pInstance;
}

void LPluginSettingsMgr::destroy()
{
    // Clean up the pointer
    if(s_pInstance != NULL) {
        delete s_pInstance, s_pInstance = NULL;
    }
}

void LPluginSettingsMgr::registerToSettingsMgr(LSettingsRelier *a_pSettingsRelier)
{
    m_settingsMgr.registerToSettingsMgr(a_pSettingsRelier);
}

void LPluginSettingsMgr::unregisterToSettingsMgr(const QString &a_rstrRegisterName)
{
    m_settingsMgr.unregisterToSettingsMgr(a_rstrRegisterName);
}

void LPluginSettingsMgr::readSettingsFile()
{
    m_settingsMgr.readSettingsFile(m_strHistoryFilePath);
}

void LPluginSettingsMgr::writeSettingsFile()
{
    m_settingsMgr.writeSettingsFile(m_strHistoryFilePath);
}

LPluginSettingsMgr::LPluginSettingsMgr()
{
    m_strHistoryFilePath = PATH_ACCESSER->getPath(ePathProject, "PluginSettingsMemory.xml");
}
