#-------------------------------------------------
#
# Project created by QtCreator 2013-12-02T09:50:41
#
#-------------------------------------------------

QT += core widgets script scripttools multimedia qml texttospeech serialport webenginewidgets printsupport


TARGET = lobster
TEMPLATE = app


SOURCES += \
    src/main.cpp \
    src/modules/data/LDataManager.cpp \
    src/modules/data/LData.cpp \
    src/modules/data/LDataBuffer.cpp \
    src/views/oscilloscope/LCursor.cpp \
    src/views/oscilloscope/qcustomplot.cpp \
    src/views/selectDlg/LSelectDataDlg.cpp \
    src/views/mainwindow/LMainWindow.cpp \
    src/views/watchwindow/LWatchWindow.cpp \
    src/views/oscilloscope/LOscilloscope.cpp \
    src/views/datalog/LSaveLog.cpp \
    src/views/oscilloscope/LPlotter.cpp \
    src/views/oscilloscope/LScopeMenu.cpp \
    src/views/oscilloscope/LLogSpinBox.cpp \
    src/modules/settings/LSettingsMgr.cpp \
    src/views/oscilloscope/LCurve.cpp \
    src/views/selectDlg/LSelectDataDlgWrapper.cpp \
    src/modules/channel/LChannel.cpp \
    src/views/channelDlg/LChannelMgrDlg.cpp \
    src/views/channelDlg/LChannelMgrDlgWrapper.cpp \
    src/views/channelDlg/LAddNewChannelDlg.cpp \
    src/modules/channel/LChannelMgr.cpp \
    src/views/watchwindow/LWatchWindowItem.cpp \
    src/views/watchwindow/LColorSelectButton.cpp \
    src/modules/datalog/LCsvHandler.cpp \
    src/views/settingsDlg/LSettingsTabDialog.cpp \
    src/modules/settings/LPluginSettingsMgr.cpp \
    src/modules/settings/LMainSettingsMgr.cpp \
    src/views/script/SMainWindow.cpp \
    src/views/script/SCodeEditor.cpp \
    src/modules/script/LScriptEvalWrapper.cpp \
    src/modules/script/LMainApi.cpp \
    src/modules/script/LPointApi.cpp \
    src/modules/script/LSystemApi.cpp \
    src/modules/script/LApi.cpp \
    src/modules/script/LLobsterApiWrapper.cpp \
    src/modules/errorhandle/LErrorHandler.cpp \
    src/views/script/SConsole.cpp \
    src/views/help/LHelpWidget.cpp \
    src/views/watchwindow/LEnumsBox.cpp \
    src/modules/common/LUsbCanDllWrapper.cpp \
    src/modules/common/LUniqueResource.cpp \
    src/modules/common/LCanDataManager.cpp \
    src/modules/common/LUniqueResource.cpp \
    src/modules/common/LUsbCanDllWrapper.cpp \
    src/modules/common/LYavUsbAdcDllWrapper.cpp \
    src/modules/common/LCanDataManager.cpp \
    src/modules/common/LUniqueResource.cpp \
    src/modules/common/LUsbAdcDataManager.cpp \
    src/modules/common/LUsbCanDllWrapper.cpp \
    src/modules/common/LYavUsbAdcDllWrapper.cpp \
    src/views/playback/LPlaybackWidget.cpp \
    src/views/playback/LPlaybackWidgetTableItem.cpp \
    src/modules/playback/LLogFileParser.cpp \
    src/views/mainwindow/LSelectProjectDialog.cpp \
    src/views/mainwindow/LSelectProjectDialog.cpp \
    src/modules/common/LDbcManagerDllWrapper.cpp \
    src/modules/common/LTmctlWrapper.cpp \
    src/modules/datalog/LTdmDllWrapper.cpp \
    src/modules/datalog/LTdmFileHandler.cpp \
    src/views/controlpage/LControlPage.cpp


HEADERS  += \
    src/modules/data/LDataManager.h \
    src/modules/data/LData.h \
    src/modules/data/LDataBuffer.h \
    src/views/oscilloscope/LCursor.h \
    src/views/oscilloscope/qcustomplot.h \
    src/views/selectDlg/LSelectDataDlg.h \
    src/views/mainwindow/LMainWindow.h \
    src/views/watchwindow/LWatchWindow.h \
    src/views/oscilloscope/LOscilloscope.h \
    src/views/datalog/LSaveLog.h \
    src/views/oscilloscope/LPlotter.h \
    src/views/oscilloscope/LScopeMenu.h \
    src/views/oscilloscope/LLogSpinBox.h \
    src/modules/settings/LSettingsMgr.h \
    src/modules/common/LSettingsRelier.h \
    src/views/oscilloscope/LCurve.h \
    src/views/selectDlg/LSelectDataDlgWrapper.h \
    src/modules/common/LTree.h \
    src/modules/common/LCommon.h \
    src/modules/common/LDataValue.h \
    src/modules/channel/LChannel.h \
    src/modules/common/LTransmission.h \
    src/modules/common/LObixObj.h \
    src/views/channelDlg/LChannelMgrDlg.h \
    src/views/channelDlg/LChannelMgrDlgWrapper.h \
    src/views/channelDlg/LAddNewChannelDlg.h \
    src/modules/channel/LChannelMgr.h \
    src/views/watchwindow/LWatchWindowItem.h \
    src/views/watchwindow/LColorSelectButton.h \
    src/modules/datalog/LCsvHandler.h \
    src/modules/common/LCommonError.h \
    src/views/settingsDlg/LSettingsTabDialog.h \
    src/modules/settings/LPluginSettingsMgr.h \
    src/modules/settings/LMainSettingsMgr.h \
    src/modules/common/LTransPluginFactory.h \
    src/views/script/SMainWindow.h \
    src/views/script/SCodeEditor.h \
    src/modules/script/LScriptEvalWrapper.h \
    src/modules/script/LMainApi.h \
    src/modules/script/LPointApi.h \
    src/modules/script/LSystemApi.h \
    src/modules/script/LApi.h \
    src/modules/script/LLobsterApiWrapper.h \
    src/modules/errorhandle/LErrorHandler.h \
    src/views/script/SConsole.h \
    src/views/help/LHelpWidget.h \
    src/modules/common/LUniqueResource.h \
    src/modules/common/LPath.h \
    src/modules/common/LAttributes.h \
    src/modules/observer/LValueObserver.h \
    src/modules/observer/LAttributesObserver.h \
    src/modules/common/LObixObjTree.h \
    src/views/watchwindow/LEnumsBox.h \
    src/modules/common/LUsbCanDllWrapper.h \
    src/modules/common/LAttributes.h \
    src/modules/common/LCanDataManager.h \
    src/modules/common/LCanDataReceiver.h \
    src/modules/common/LCommon.h \
    src/modules/common/LCommonError.h \
    src/modules/common/LDataValue.h \
    src/modules/common/LObixObj.h \
    src/modules/common/LObixObjTree.h \
    src/modules/common/LPath.h \
    src/modules/common/LSettingsRelier.h \
    src/modules/common/LTransmission.h \
    src/modules/common/LTransPluginFactory.h \
    src/modules/common/LTree.h \
    src/modules/common/LUniqueResource.h \
    src/modules/common/LUsbCanDllWrapper.h \
    src/modules/common/LYavUsbAdcDllWrapper.h \
    src/modules/common/LAttributes.h \
    src/modules/common/LCanDataManager.h \
    src/modules/common/LCanDataReceiver.h \
    src/modules/common/LCommon.h \
    src/modules/common/LCommonError.h \
    src/modules/common/LDataValue.h \
    src/modules/common/LObixObj.h \
    src/modules/common/LObixObjTree.h \
    src/modules/common/LPath.h \
    src/modules/common/LSettingsRelier.h \
    src/modules/common/LTransmission.h \
    src/modules/common/LTransPluginFactory.h \
    src/modules/common/LTree.h \
    src/modules/common/LUniqueResource.h \
    src/modules/common/LUsbAdcDataManager.h \
    src/modules/common/LUsbAdcDataReceiver.h \
    src/modules/common/LUsbCanDllWrapper.h \
    src/modules/common/LYavUsbAdcDllWrapper.h \
    src/views/playback/LPlaybackWidget.h \
    src/views/playback/LPlaybackWidgetTableItem.h \
    src/modules/playback/LLogFileParser.h \
    src/views/mainwindow/LSelectProjectDialog.h \
    src/views/mainwindow/LSelectProjectDialog.h \
    src/modules/common/LDbcManagerDllWrapper.h \
    src/modules/common/LTmctlWrapper.h \
    src/modules/datalog/LTdmDllWrapper.h \
    src/modules/datalog/LTdmFileHandler.h \
    src/modules/datalog/LDataLogHangler.h \
    src/views/controlpage/LControlPage.h

FORMS    += \
    src/views/selectDlg/LSelectDataDlg.ui \
    src/views/mainwindow/LMainWindow.ui \
    src/views/watchwindow/LWatchWindow.ui \
    src/views/oscilloscope/LOscilloscope.ui \
    src/views/datalog/LSaveLog.ui \
    src/views/oscilloscope/LScopeMenu.ui \
    src/views/channelDlg/LAddNewChannelDlg.ui \
    src/views/channelDlg/LChannelMgrDlg.ui \
    src/views/watchwindow/LColorSelectButton.ui \
    src/views/settingsDlg/LSettingsTabDialog.ui \
    src/views/script/SMainWindow.ui \
    src/views/help/LHelpWidget.ui \
    src/views/playback/LPlaybackWidget.ui \
    src/views/mainwindow/LSelectProjectDialog.ui \
    src/views/mainwindow/LSelectProjectDialog.ui \
    src/views/controlpage/LControlPage.ui

RC_FILE = \
    resource/lobsterico.rc

RESOURCES +=

INCLUDEPATH += \
    src/modules \
    src/modules/data \
    src/modules/channel \
    src/views \
    src/modules/settings \
    src/views/selectDlg \
    src/views/channelDlg \
    src/views/settingsDlg \
    src/views/oscilloscope \
    src/views/script \
    src/views/watchwindow \
    src/views/datalog \
    src/modules/common \
    src/modules/script \
    src/modules/errorhandle \
    src/views/help \
    src/modules/path \
    src/modules/observer \
    src/views/mainwindow \
    src/modules/datalog \
    src/views/playback \
    src/modules/playback \
    src/views/controlpage \
