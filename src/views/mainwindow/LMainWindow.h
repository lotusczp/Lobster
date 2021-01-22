#ifndef LMAINWINDOW_H
#define LMAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QCloseEvent>


#include "LWatchWindow.h"
#include "LOscilloscope.h"
#include "LSaveLog.h"
#include "LSelectDataDlgWrapper.h"
#include "LChannelMgrDlgWrapper.h"
#include "LSettingsTabDialog.h"
#include "LPluginSettingsMgr.h"
#include "SMainWindow.h"
#include "LHelpWidget.h"
#include "LControlPage.h"
#include "LPlaybackWidget.h"

namespace Ui {
class LMainWindow;
}

class LMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LMainWindow(QWidget *parent = 0);
    ~LMainWindow();

    void handlePlayerState();

protected:
    virtual void closeEvent(QCloseEvent *event);

public slots:
    void receiveScopeEnableState(QString name, bool state);

    void receiveCurveColor(QString name, QColor curveColor);
    void receiveCurveSelected(QString name, bool selected);
    void receiveMergeValueAxis(QList<QString> names);
    void receiveDemergeValueAxis(QList<QString> names);

    void receiveLogEnableState(QString name, bool state);

    void receiveDeletedChannels(LDeletedChannelList a_listChannelNames);

    void handleMainApiCmd(bool command);

    void handlePointApiCmd(bool command, QString pointName);

private slots:
    void playPauseAllChannels();

    void stopRunAllChannels();

    void openAllSettingsWindow();

    void openToolWindow();

    void openScriptIde();

    void openPlaybackWindow();

    void openControlPage();


    void on_actionSelect_Data_triggered();

    void on_actionManager_triggered();

    void on_actionContents_triggered();

    void on_actionAbout_triggered();

    void on_actionSave_Project_triggered();

private:
    void initialize();

    void createToolBar();

    void createMenu();

    void popSelectDataDlg();

    void popChannelMgrDlg();

    void createSettingTabWidget();

    void addToolWidget();


    void readWindowsSettings();

    void writeWindowsSettings();

    void acceptSelection(QList<QString> a_listNames);


private:
    Ui::LMainWindow *ui;

    enum PlayerState {
        play,
        pause,
        stop
    };

    PlayerState m_channelState;

    // toolbar
    QToolButton* m_pStopButton;
    QToolButton* m_pRunButton;
    QToolButton* m_pConfigButton;

    // observer
    LWatchWindow* m_pWatchWindow;
    LOscilloscope* m_pOscilloscope;
    LSaveLog* m_pLog;

    // dialog wrappers
    LSelectDataDlgWrapper* m_pSelectDataDlgWrapper;
    LChannelMgrDlgWrapper* m_pChannelMgrDlgWrapper;
    LSettingsTabDialog* m_pSettingTabWidget;

    // script related
    SMainWindow* m_pSMainWindow;

    // Playback window
    LPlaybackWidget *m_pPlaybackWidget;

    // Control page
    LControlPage *m_pControlPage;

    // help contents widget
    LHelpWidget* m_pHelpWidget;
};

#endif // LMAINWINDOW_H
