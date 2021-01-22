#ifndef LPLUGINSETTINGSMGR_H
#define LPLUGINSETTINGSMGR_H

#include "LSettingsMgr.h"

class LPluginSettingsMgr
{
public:
    static LPluginSettingsMgr* instance();

    static void destroy();


    //! all the objects which need to save settings should inherit LSettingsRelier and register to this manager
    void registerToSettingsMgr(LSettingsRelier* a_pSettingsRelier);

    //! all the objects which need to save settings should inherit LSettingsRelier and register to this manager
    void unregisterToSettingsMgr(const QString &a_rstrRegisterName);


    //! read and dispatch setting tree
    void readSettingsFile();

    //! write setting tree to the xml file
    void writeSettingsFile();

private:
    LPluginSettingsMgr();

private:
    static LPluginSettingsMgr  *s_pInstance;

    QString m_strHistoryFilePath;

    LSettingsMgr m_settingsMgr;
};

#endif // LPLUGINSETTINGSMGR_H
