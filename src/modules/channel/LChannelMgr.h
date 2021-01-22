#ifndef LCHANNELMGR_H
#define LCHANNELMGR_H

#include <QMap>


#include "LChannel.h"
#include "LDataValue.h"

class LChannelMgr : public QObject
{
    Q_OBJECT

public:
    static LChannelMgr* instance();

    static void destroy();

    //! add a new transmission protocol
    void addChannel(const QString& a_rChannelName, const QString& a_rCfgPath, LTransmission& a_rTransmission);

    //! delete the existed protocol
    void deleteChannel(const QString& a_rChannelName);



    //! return the path of config file of this protocol
    QString getCfgPathByChannel(const QString& a_rChannelName) const;

    //! set the path of config file of this protocol
    void setCfgPathByChannel(const QString& a_rChannelName, const QString& a_rConfigPath);



    //! Activate the protocol by user
    void activateChannel(const QString& a_rChannelName);

    //! Deactivate the protocol by user
    void deactivateChannel(const QString& a_rChannelName);

    //! is this protocol activated by user
    bool isChannelActive(const QString& a_rChannelName) const;



    //! start all the transmission activated by user
    void startActiveChannels();

    //! stop all the transmission activated by user
    void stopActiveChannels();



    //! return all the protocols for other module to traverse
    QList<QString> getChannelList() const;



    //! set the Transmission Action Poiter
    void setTransmissionPtr(const QString &a_rChannelName, LTransmission& a_rTransmission);



    //! parse all config files and config all channels
    void configurateAllChannels();

    //! reset the cared data list
    void resetCaredDataList(const QString& a_rChannelName, const QList<QString> &a_rCaredDataList);

    //! get the whole data list of a channel
    bool getDataListByChannel(const QString& a_rChannelName, QList<QString> &a_rDataList) const;

    //! get the whole data list of all channels
    void getAvailableDatas(QList<QString> &a_rWholeDataList) const;


    //! get the setting widgets by channel name
    QWidget* getSettingsWidget(const QString& a_rChannelName);

    //! get the setting widgets by channel name
    QWidget* getToolWidget(const QString& a_rChannelName);

    //! Pull attributes from all channels
    void pullAttriFromAllChannels();


private:
    LChannelMgr();
    ~LChannelMgr();

    void clearChannelMap();

    //! set the iterator of the configuration tree for a single channel
    void configChannel(const QString& a_rChannelName, const QString &a_rFilename);




private:
    static LChannelMgr  *s_pInstance;

    QMap<QString, LChannel*> m_mapChannels;   //!< [Channel name][Channel]

    LTree<LObixObj>         *m_pObixTree;                 //!< obix tree for all the config files
};

#endif // LCHANNELMGR_H
