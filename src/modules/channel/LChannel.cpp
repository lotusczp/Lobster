#include "LChannel.h"

LChannel::LChannel()
{
} 

LChannel::LChannel(const QString &a_rChannelName, const QString &a_rCfgPath, LTransmission &a_rTransmission)
{
    m_strChannelName = a_rChannelName;
    m_strCfgPath = a_rCfgPath;
    m_fIsActive  = false;
    m_pTransmission = &a_rTransmission;
}

LChannel::~LChannel()
{
    if (m_pTransmission != NULL) {
        m_pTransmission->stop();

        delete m_pTransmission, m_pTransmission = NULL;
    }
}

void LChannel::setActive(bool a_fIsActive)
{
    // set flag
    m_fIsActive = a_fIsActive;
}

bool LChannel::isActive() const
{
    return m_fIsActive;
}

void LChannel::setTransmissionPtr(LTransmission& a_rTransmission)
{
    if (m_pTransmission != NULL) {
        m_pTransmission->stop();

        delete m_pTransmission, m_pTransmission = NULL;
    }
    m_pTransmission = &a_rTransmission;
}

const LTransmission *LChannel::getTransmissionPtr()
{
    return m_pTransmission;
}

void LChannel::setCfgPath(const QString &a_rCfgPath)
{
    m_strCfgPath = a_rCfgPath;
}

QString LChannel::getCfgPath() const
{
    return m_strCfgPath;
}

void LChannel::configChannel(const LObixObjIter &a_rConfigIter)
{
    m_pTransmission->configurate(a_rConfigIter);
}

bool LChannel::getDataList(QList<QString> &a_rDataList) const
{
    return m_pTransmission->getDataList(a_rDataList);
}

void LChannel::resetCaredDataList(const QList<QString> &a_rCaredDataList)
{
    m_pTransmission->resetCaredDataList(a_rCaredDataList);
}

void LChannel::startTransmission()
{
    m_pTransmission->start();
}

void LChannel::stopTransmission()
{
    m_pTransmission->stop();
}
  
QWidget *LChannel::getSettingsWidget()
{
    return m_pTransmission->getSettingsWidget();
}

QWidget *LChannel::getToolWidget()
{
    return m_pTransmission->getToolWidget();
}

void LChannel::pullAttributes()
{
    m_pTransmission->pullAttributes();
}
