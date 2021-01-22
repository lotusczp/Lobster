#include "LTdmDllWrapper.h"
#include <QMessageBox>

LTdmDllWrapper* LTdmDllWrapper::s_pInstance = nullptr;

LTdmDllWrapper::LTdmDllWrapper()
    : m_library("./thirdparty/nilibddc.dll")
{
    createFile = (DDC_CreateFile*)m_library.resolve("DDC_CreateFile");
    addChannelGroup = (DDC_AddChannelGroup*)m_library.resolve("DDC_AddChannelGroup");
    addChannel = (DDC_AddChannel*)m_library.resolve("DDC_AddChannel");
    saveFile = (DDC_SaveFile*)m_library.resolve("DDC_SaveFile");
    closeFile = (DDC_CloseFile*)m_library.resolve("DDC_CloseFile");
    removeChannelGroup = (DDC_RemoveChannelGroup*)m_library.resolve("DDC_RemoveChannelGroup");
    removeChannel = (DDC_RemoveChannel*)m_library.resolve("DDC_RemoveChannel");
    closeChannelGroup = (DDC_CloseChannelGroup*)m_library.resolve("DDC_CloseChannelGroup");
    closeChannel = (DDC_CloseChannel*)m_library.resolve("DDC_CloseChannel");
    setDataValues = (DDC_SetDataValues*)m_library.resolve("DDC_SetDataValues");
    setDataValuesTimestampComponents = (DDC_SetDataValuesTimestampComponents*)m_library.resolve("DDC_SetDataValuesTimestampComponents");
    appendDataValues = (DDC_AppendDataValues*)m_library.resolve("DDC_AppendDataValues");
    appendDataValuesTimestampComponents = (DDC_AppendDataValuesTimestampComponents*)m_library.resolve("DDC_AppendDataValuesTimestampComponents");
    getChannels = (DDC_GetChannels*)m_library.resolve("DDC_GetChannels");
    openFileEx = (DDC_OpenFileEx*)m_library.resolve("DDC_OpenFileEx");
    getNumChannelGroups = (DDC_GetNumChannelGroups*)m_library.resolve("DDC_GetNumChannelGroups");
    getChannelGroups = (DDC_GetChannelGroups*)m_library.resolve("DDC_GetChannelGroups");
    getChannelGroupStringPropertyLength = (DDC_GetChannelGroupStringPropertyLength*)m_library.resolve("DDC_GetChannelGroupStringPropertyLength");
    getChannelGroupProperty = (DDC_GetChannelGroupProperty*)m_library.resolve("DDC_GetChannelGroupProperty");
    getNumChannels = (DDC_GetNumChannels*)m_library.resolve("DDC_GetNumChannels");
    getNumDataValues = (DDC_GetNumDataValues*)m_library.resolve("DDC_GetNumDataValues");
    getDataValues = (DDC_GetDataValues*)m_library.resolve("DDC_GetDataValues");
    getDataValuesDouble = (DDC_GetDataValuesDouble*)m_library.resolve("DDC_GetDataValuesDouble");
    getDataValuesTimestampComponents = (DDC_GetDataValuesTimestampComponents*)m_library.resolve("DDC_GetDataValuesTimestampComponents");
    freeMemory = (DDC_FreeMemory*)m_library.resolve("DDC_FreeMemory");

    if(createFile == nullptr
            || addChannelGroup == nullptr
            || addChannel == nullptr
            || saveFile == nullptr
            || closeFile == nullptr
            || removeChannelGroup == nullptr
            || removeChannel == nullptr
            || closeChannelGroup == nullptr
            || closeChannel == nullptr
            || setDataValues == nullptr
            || setDataValuesTimestampComponents == nullptr
            || appendDataValues == nullptr
            || appendDataValuesTimestampComponents == nullptr
            || getChannels == nullptr
            || openFileEx == nullptr
            || getNumChannelGroups == nullptr
            || getChannelGroups == nullptr
            || getChannelGroupStringPropertyLength == nullptr
            || getChannelGroupProperty == nullptr
            || getNumChannels == nullptr
            || getNumDataValues == nullptr
            || getDataValues == nullptr
            || getDataValuesDouble == nullptr
            || getDataValuesTimestampComponents == nullptr
            || freeMemory == nullptr
            ) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"), "Can not parse nilibddc.dll, please check whether it exists in ./thirdparty/ ");
        QMessageBox::warning(nullptr, QObject::tr("Warning"), m_library.errorString());

        exit(-1);
    }
}
