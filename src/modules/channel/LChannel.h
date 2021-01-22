#ifndef LCHANNEL_H
#define LCHANNEL_H

#include <QString>

#include "LTransmission.h"

class LChannel
{
public:
    LChannel(const QString& a_rChannelName, const QString& a_rCfgPath, LTransmission& a_rTransmission);
    ~LChannel();

    void setActive(bool a_fIsActive);

    bool isActive() const;


    void setTransmissionPtr(LTransmission& a_rTransmission);

    //! return the const pointer of transmission
    const LTransmission* getTransmissionPtr();


    void setCfgPath(const QString& a_rCfgPath);

    QString getCfgPath() const;


    //! set the iterator of the configuration tree for this channel
    void configChannel(const LObixObjIter &a_rConfigIter);

    //! get the whole data list of a channel
    bool getDataList(QList<QString> &a_rDataList) const;

    //! reset the cared data list
    void resetCaredDataList(const QList<QString> &a_rCaredDataList);


    //! start the transmission
    void startTransmission();

    //! stop the transmission, this will release the hardware resource which was occupied by transmission
    void stopTransmission();


    //! get the setting widget of this channel
    QWidget* getSettingsWidget();

    //! get the setting widget of this channel
    QWidget* getToolWidget();

    //! Force to pull attributes from this channel
    void pullAttributes();

private:
    LChannel();

private:
    QString m_strChannelName;  //!< name of this transmission

    QString m_strCfgPath;   //!< path of config file

    bool m_fIsActive;   //!< whether it's activated by user

    LTransmission* m_pTransmission;  //!< the action impelemetion instance for this trans
};

#endif // LCHANNEL_H
