#include "LTdmFileHandler.h"
#include <QDateTime>
#include <QDebug>

LTdmFileHandler::LTdmFileHandler()
{
    m_fileHandle = nullptr;
}

bool LTdmFileHandler::openFile()
{
    QString strFile = "Lobster_Datalog_" + QDateTime::currentDateTime().toString("yyyyMMdd") + "_" + QDateTime::currentDateTime().toString("hh:mm:ss").remove(QChar(':')) + ".tdm";
    QString strFilePath = ".\\datalog\\" + strFile;

    int iResultFile = LTdmDllWrapper::instance()->createFile(strFilePath.toLocal8Bit().constData(),
                                                        "TDM",
                                                        "Lobster data file",
                                                        "Data file created by Lobster main program",
                                                        "",
                                                        "",
                                                        &m_fileHandle);

    return iResultFile==DDC_NoError;
}

bool LTdmFileHandler::saveFile()
{
    int iResultFile = LTdmDllWrapper::instance()->saveFile(m_fileHandle);

    return iResultFile==DDC_NoError;
}

bool LTdmFileHandler::closeFile()
{
    int iResultFile = LTdmDllWrapper::instance()->closeFile(m_fileHandle);

    return iResultFile==DDC_NoError;
}

void LTdmFileHandler::addNewLine(const QStringList &a_rstrLineContent)
{
    // |name | value | abs time
    if(a_rstrLineContent.size() != 3) return; // Error

    QString strVarName = a_rstrLineContent[0];
    DDCChannelHandle timeChannel = nullptr;
    DDCChannelHandle valueChannel = nullptr;

    if(m_mapGroupHandles.contains(strVarName)) {
        DDCChannelHandle channelsBuffer[2];
        int iResultGetChannels = LTdmDllWrapper::instance()->getChannels(m_mapGroupHandles[strVarName],
                                                                         channelsBuffer,
                                                                         2);
        if(iResultGetChannels != DDC_NoError) return; // Error, do nothing

        // Assign channels
        timeChannel = channelsBuffer[0];
        valueChannel = channelsBuffer[1];
    }
    else {
        // No this channel yet, create it
        m_mapGroupHandles[strVarName] = nullptr;

        int iResultAddGroup = LTdmDllWrapper::instance()->addChannelGroup(m_fileHandle,
                                                                          strVarName.toLocal8Bit().constData(),
                                                                          "",
                                                                          &m_mapGroupHandles[strVarName]);
        if(iResultAddGroup != DDC_NoError) {
            m_mapGroupHandles.remove(strVarName); // First remove this item
            return; // Error, do nothing
        }

        // Add timestamp channel
        int iResultAddTimeChannel = LTdmDllWrapper::instance()->addChannel(m_mapGroupHandles[strVarName],
                                                                           DDC_Timestamp,
                                                                           "time",
                                                                           "",
                                                                           "",
                                                                           &timeChannel);
        if(iResultAddTimeChannel != DDC_NoError) return; // Error, do nothing

        // The value channel's data type must be double
        int iResultAddValueChannel = LTdmDllWrapper::instance()->addChannel(m_mapGroupHandles[strVarName],
                                                                            DDC_Double,
                                                                            "value",
                                                                            "",
                                                                            "",
                                                                            &valueChannel);
        if(iResultAddValueChannel != DDC_NoError) return; // Error, do nothing
    }

    qint64 iAbsTime = a_rstrLineContent[2].toLongLong();
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(iAbsTime);
    unsigned int year[1] = {dateTime.toString("yyyy").toUInt()};
    unsigned int month[1] = {dateTime.toString("M").toUInt()};
    unsigned int day[1] = {dateTime.toString("d").toUInt()};
    unsigned int hour[1] = {dateTime.toString("H").toUInt()};
    unsigned int minute[1] = {dateTime.toString("m").toUInt()};
    unsigned int second[1] = {dateTime.toString("s").toUInt()};
    qint64 tmp = iAbsTime % 1000;
    double  millisecond[1] = {(double)tmp,}; // There are issues if we don't do this

    int iResultAppendTime = LTdmDllWrapper::instance()->appendDataValuesTimestampComponents(timeChannel,
                                                                                            year,
                                                                                            month,
                                                                                            day,
                                                                                            hour,
                                                                                            minute,
                                                                                            second,
                                                                                            millisecond,
                                                                                            1);
    if(iResultAppendTime != DDC_NoError) return; // Error, do nothing

    double dValue = a_rstrLineContent[1].toDouble();

    int iResultAppendValue = LTdmDllWrapper::instance()->appendDataValues(valueChannel,
                                                                          &dValue,
                                                                          1);
    if(iResultAppendValue != DDC_NoError) return; // Error, do nothing
}

void LTdmFileHandler::flush()
{
    // Do nothing
}
