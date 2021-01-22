#ifndef LMAINSETTINGSMGR_H
#define LMAINSETTINGSMGR_H

#include "LSettingsMgr.h"

class LMainSettingsMgr
{
public:
    static LMainSettingsMgr* instance();

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
    LMainSettingsMgr();

private:
    static LMainSettingsMgr  *s_pInstance;

    QString m_strHistoryFilePath;

    LSettingsMgr m_settingsMgr;

};

#endif // LMAINSETTINGSMGR_H
