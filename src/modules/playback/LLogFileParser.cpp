#include "LLogFileParser.h"

#include <QDebug>
#include "LTdmDllWrapper.h"


LLogFileParser::LLogFileParser(QObject *parent) : QObject(parent)
{

}

void LLogFileParser::parseFile(QString filePath)
{
    DDCFileHandle fileHandle;
    int iResultOpen = LTdmDllWrapper::instance()->openFileEx(filePath.replace("/", "\\").toLocal8Bit().constData(), "TDM", 1, &fileHandle);
    if(iResultOpen != DDC_NoError) return;

    // Check the groups, each point correspondes to a group
    unsigned int uiGroupNum = 0;
    int iResultGetGroupNum = LTdmDllWrapper::instance()->getNumChannelGroups(fileHandle, &uiGroupNum);
    if(iResultGetGroupNum != DDC_NoError) return;

    DDCChannelGroupHandle *groups = new DDCChannelGroupHandle[uiGroupNum];
    int iResultGetGroups = LTdmDllWrapper::instance()->getChannelGroups(fileHandle, groups, uiGroupNum);
    if(iResultGetGroups != DDC_NoError) uiGroupNum = 0;

    // Parse and send all names first
    QStringList valueNames;
    for(unsigned int groupIndex=0; groupIndex<uiGroupNum; ++groupIndex) {
        unsigned int length = 0;
        int iResultGetGroupsPropLen = LTdmDllWrapper::instance()->getChannelGroupStringPropertyLength(groups[groupIndex], DDC_CHANNELGROUP_NAME, &length);
        if(iResultGetGroupsPropLen != DDC_NoError) break;
        char* chGroupName = new char[length+1];
        int iResultGetGroupsProp = LTdmDllWrapper::instance()->getChannelGroupProperty(groups[groupIndex], DDC_CHANNELGROUP_NAME, chGroupName, length + 1);
        if(iResultGetGroupsProp != DDC_NoError) {
            delete[] chGroupName;
            break;
        }
        QString strGroupName(chGroupName);
        valueNames << strGroupName;
    }
    emit sendNames(valueNames);

    for(unsigned int groupIndex=0; groupIndex<uiGroupNum; ++groupIndex) {
        unsigned int length = 0;
        int iResultGetGroupsPropLen = LTdmDllWrapper::instance()->getChannelGroupStringPropertyLength(groups[groupIndex], DDC_CHANNELGROUP_NAME, &length);
        if(iResultGetGroupsPropLen != DDC_NoError) break;
        char* chGroupName = new char[length+1];
        int iResultGetGroupsProp = LTdmDllWrapper::instance()->getChannelGroupProperty(groups[groupIndex], DDC_CHANNELGROUP_NAME, chGroupName, length + 1);
        if(iResultGetGroupsProp != DDC_NoError) {
            delete[] chGroupName;
            break;
        }
        QString strGroupName(chGroupName);
        // Read channels
        unsigned int numChannels = 0;
        int iResultGetChNum = LTdmDllWrapper::instance()->getNumChannels(groups[groupIndex], &numChannels);
        if(iResultGetChNum != DDC_NoError || numChannels != 2) break;
        DDCChannelHandle *channels = new DDCChannelHandle[numChannels];
        int iResultGetChannels = LTdmDllWrapper::instance()->getChannels(groups[groupIndex], channels, numChannels);
        if(iResultGetChannels != DDC_NoError) {
            delete[] channels;
            break;
        }
        // Must have 2 channels, "time" and "value"
        // We don't check the channel name here to make it simple
        unsigned __int64 numDataTimes = 0;
        unsigned __int64 numDataValues = 0;
        int iResultGetTimeNum = LTdmDllWrapper::instance()->getNumDataValues(channels[0], &numDataTimes);
        int iResultGetValueNum = LTdmDllWrapper::instance()->getNumDataValues(channels[1], &numDataValues);
        if(iResultGetTimeNum == DDC_NoError
                && iResultGetValueNum == DDC_NoError
                && numDataTimes == numDataValues) {
            for(unsigned __int64 index = 0; index < numDataTimes; ++index) {
                // Get the time channel
                unsigned int year[1] = {0};
                unsigned int month[1] = {0};
                unsigned int day[1] = {0};
                unsigned int hour[1] = {0};
                unsigned int minute[1] = {0};
                unsigned int second[1] = {0};
                double  millisecond[1] = {0};
                unsigned int weekDay[1] = {0};
                LTdmDllWrapper::instance()->getDataValuesTimestampComponents(channels[0], index, 1, year, month, day, hour, minute, second, millisecond, weekDay);
                QTime time(hour[0], minute[0], second[0], (int)millisecond[0]);
                QDate date(year[0], month[0], day[0]);
                QDateTime datetime(date, time);
                qint64 iAbsTime = datetime.toMSecsSinceEpoch();
                // Get the value channel
                double values[1];
                LTdmDllWrapper::instance()->getDataValuesDouble(channels[1], index, 1, &values[0]);
                // Send the value
                LDataValue dataValue;
                dataValue.set(values[0], iAbsTime);
                emit sendDataValue(strGroupName, dataValue);
                double channelPer = ((double)(index+1)/numDataTimes);
                emit sendProcessPercent((int)(((double)groupIndex+channelPer)/uiGroupNum*100));
            }
        }
        // Free memory
        delete[] channels;
        delete[] chGroupName;
    }
    // Free memory
    delete[] groups;

    emit sendProcessFinished();

    // Close file when finished
    LTdmDllWrapper::instance()->closeFile(fileHandle);
}
