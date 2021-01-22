#ifndef LSETTINGSMGR_H
#define LSETTINGSMGR_H

#include <QMap>

#include "LSettingsRelier.h"

class LSettingsMgr
{
public:
    LSettingsMgr();
    ~LSettingsMgr();

    //! all the objects which need to save settings should inherit LSettingsRelier and register to this manager
    void registerToSettingsMgr(LSettingsRelier* a_pSettingsRelier);

    //! all the objects which need to save settings should inherit LSettingsRelier and register to this manager
    void unregisterToSettingsMgr(const QString& a_rstrRegisterName);


    //! read and dispatch setting tree
    void readSettingsFile(const QString& a_rstrFilePath);

    //! write setting tree to the xml file
    void writeSettingsFile(const QString& a_rstrFilePath);

private:
    QMap<QString, LSettingsRelier*> m_mapReliers;   //!< map of all registered settings relier

    LTree<LObixObj> *m_pSettingsTree; //!< tree of all settings
};

#endif // LSETTINGSMGR_H
