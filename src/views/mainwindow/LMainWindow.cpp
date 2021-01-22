#include "LMainWindow.h"
#include "ui_LMainWindow.h"

#include <QSettings>
#include <QFile>
#include <QMessageBox>

#include "LDataManager.h"
#include "LChannelMgrDlg.h"
#include "LChannelMgr.h"
#include "LMainSettingsMgr.h"
#include "LPluginSettingsMgr.h"
#include "LErrorHandler.h"
#include "LUniqueResource.h"

const QString c_strVersion = "1.2.1";

LMainWindow::LMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LMainWindow)
{
    ui->setupUi(this);

    ui->centralwidget->hide();
    setDockNestingEnabled(true);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);

    // initialize all components
    initialize();

    // read the windows layout and size information .etc
    readWindowsSettings();

    // fake a dialog of channel manager and accept it
    popChannelMgrDlg();
    m_pChannelMgrDlgWrapper->clickOk();
#if 0
    // apply the change of channel, create all available data names
    LDataManager::instance()->fetchAllAvailableDataNames();
    // set the list name to the auto-complete dict in code editor during the first load
    m_pSMainWindow->setAutoCompModelStrings(LDataManager::instance()->getDataNameCollection());
#endif

    // read all channels settings and apply them
    LPluginSettingsMgr::instance()->readSettingsFile();

    // fake a dialog of selecting the data and accept it
    popSelectDataDlg();
    m_pSelectDataDlgWrapper->clickOk();

    // initialize settings tab widget
    /// \note this should be called after all the plugins are loaded
    createSettingTabWidget();

    // handle all the channel widgets of channels
    addToolWidget();

    // create help widget
    m_pHelpWidget = new LHelpWidget; // not modal widget, delete it in destructor

    // Create control page
    m_pControlPage = new LControlPage;

    // Create playback widget
    m_pPlaybackWidget = new LPlaybackWidget;
}

LMainWindow::~LMainWindow()
{
    // Delete all singleton modules
    LDataManager::destroy();
    LChannelMgr::destroy();
    LMainSettingsMgr::destroy();
    LPluginSettingsMgr::destroy();
    LErrorHandler::destroy();
    LUniqueResourceMgr::destroy();

    // ------------------------- delete ui related --------------------------//
    // delete dialogs
    if (m_pSelectDataDlgWrapper != nullptr) {
        delete m_pSelectDataDlgWrapper;
        m_pSelectDataDlgWrapper = nullptr;
    }
    if (m_pChannelMgrDlgWrapper != nullptr) {
        delete m_pChannelMgrDlgWrapper;
        m_pChannelMgrDlgWrapper = nullptr;
    }

    // delete script IDE
    if (m_pSMainWindow != nullptr) {
        delete m_pSMainWindow;
        m_pSMainWindow = nullptr;
    }

    // delete help widget
    delete m_pHelpWidget;
    m_pHelpWidget = nullptr;

    delete m_pControlPage;
    m_pControlPage = nullptr;

    delete m_pPlaybackWidget;
    m_pPlaybackWidget = nullptr;

    // delete all ui left
    delete ui;
}

void LMainWindow::playPauseAllChannels()
{
    if(m_channelState != play) {
        m_channelState = play;
    }
    else {
        m_channelState = pause;
    }
    handlePlayerState();
}

void LMainWindow::stopRunAllChannels()
{
    m_channelState = stop;
    handlePlayerState();
}

void LMainWindow::openAllSettingsWindow()
{
    if( (m_pSettingTabWidget->isHidden()) && (m_pSettingTabWidget->count() != 0) ) {
        m_pSettingTabWidget->exec();
    }
}

void LMainWindow::openToolWindow()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        LChannelMgr::instance()->getToolWidget(action->text())->setWindowTitle(action->text());
        LChannelMgr::instance()->getToolWidget(action->text())->show();
    }

}

void LMainWindow::openScriptIde()
{
    if (m_pSMainWindow->isHidden()) {
        m_pSMainWindow->show();
    }
}

void LMainWindow::openPlaybackWindow()
{
    if (m_pPlaybackWidget->isHidden()) {
        m_pPlaybackWidget->show();
    }
}

void LMainWindow::openControlPage()
{
    if(m_pControlPage->isHidden()) {
        m_pControlPage->setControlPage(PATH_ACCESSER->getPath(ePathProject, "controlpage/index.html"));
        m_pControlPage->show();
    }
}

void LMainWindow::handlePlayerState()
{
    LValueObserver* pOscilloscope = m_pOscilloscope;
    LValueObserver* pWatchWindow = m_pWatchWindow;
    LValueObserver* pDatalog = m_pLog;

    switch(m_channelState)
    {
    case play:
        // start the transmission threads that include these data
        LDataManager::instance()->startActiveChannels();

        // change the button to pause button
        m_pRunButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "Pause-Normal-Red-icon.png")));
        m_pRunButton->setToolTip(tr("Click to Pause all selected transmission"));
        // Transmission running
        m_pStopButton->setEnabled(true);
        // hardware config is disabled
        m_pConfigButton->setDisabled(true);
        // disable channel reconfig
        ui->actionManager->setEnabled(false);
        // disable select dialog
#if 0
        ui->actionSelect_Data->setEnabled(false);
#endif


        // start observers
        pOscilloscope->start();
        pWatchWindow->start();
        pDatalog->start();
        break;
    case pause:
        // pause the transmission threads that include these data
        LDataManager::instance()->pauseActiveChannels();

        // change the button to run button
        m_pRunButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "start-icon.png")));
        m_pRunButton->setToolTip(tr("Click to run all selected transmission"));
        // Transmission is running
        m_pStopButton->setEnabled(true);
        // hardware config is disabled
        m_pConfigButton->setDisabled(true);
        // disable channel reconfig
        ui->actionManager->setEnabled(false);
        // disable select dialog
        ui->actionSelect_Data->setEnabled(false);

        // stop observers
        pOscilloscope->stop();
        //pWatchWindow->stop();

        break;
    case stop:
        // stop the transmission threads that include these data
        LDataManager::instance()->stopActiveChannels();

        // change the button to run button
        m_pRunButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "start-icon.png")));
        m_pRunButton->setToolTip(tr("Click to run all selected transmission"));
        // Transmission is clicked to stop
        m_pStopButton->setDisabled(true);
        // hardware config is enabled
        m_pConfigButton->setEnabled(true);
        // disable channel reconfig
        ui->actionManager->setEnabled(true);
        // disable select dialog
        ui->actionSelect_Data->setEnabled(true);

        // stop observers
        pOscilloscope->stop();
        pWatchWindow->stop();
        pDatalog->stop();
        break;
    default:
        break;
    }
}

void LMainWindow::closeEvent(QCloseEvent *event)
{
    // -------------- stop all channels if not stopped yet -----------------//
    LDataManager::instance()->stopActiveChannels();

    // ---------------------- close all non-child widgets ------------------ //
    QListIterator<QString> i(LChannelMgr::instance()->getChannelList());
    while(i.hasNext()) {
        QString strChannelName = i.next();
        QWidget* pSettingsWidget = LChannelMgr::instance()->getToolWidget(strChannelName);
        if (pSettingsWidget != nullptr) {
            // close if opened
            if (!pSettingsWidget->isHidden()) {
                pSettingsWidget->close();
            }
        }
    }

    // close js ide, this is also a non-child widget
    if (m_pSMainWindow) {
        if (!m_pSMainWindow->isHidden()) {
            m_pSMainWindow->close();
        }
    }

    // accept close event
    event->accept();
}

void LMainWindow::receiveScopeEnableState(QString name, bool state)
{
    if(state) {
        LValueObserver* pOscilloscope = m_pOscilloscope;
        LDataManager::instance()->attachValueObserver(name, pOscilloscope);
        m_pOscilloscope->addNewCurve(name);
    }
    else {
        LValueObserver* pOscilloscope = m_pOscilloscope;
        LDataManager::instance()->detachValueObserver(name, pOscilloscope);
        m_pOscilloscope->eraseCurve(name);
    }
}

void LMainWindow::receiveCurveColor(QString name, QColor curveColor)
{
    m_pOscilloscope->setCurveColor(name, curveColor);
}

void LMainWindow::receiveCurveSelected(QString name, bool selected)
{
    m_pOscilloscope->setCurveSelected(name, selected);
}

void LMainWindow::receiveMergeValueAxis(QList<QString> names)
{
    m_pOscilloscope->mergeValueAxis(names);
}

void LMainWindow::receiveDemergeValueAxis(QList<QString> names)
{
    m_pOscilloscope->demergeValueAxis(names);
}

void LMainWindow::receiveLogEnableState(QString name, bool state)
{
    if(state) {
        LValueObserver* pLog = m_pLog;
        LDataManager::instance()->attachValueObserver(name, pLog);
    }
    else {
        LValueObserver* pLog = m_pLog;
        LDataManager::instance()->detachValueObserver(name, pLog);
    }
}

void LMainWindow::receiveDeletedChannels(LDeletedChannelList a_listChannelNames)
{
    // delete tool menu if exists
    QList<QAction*> listActions = ui->menuTools->actions();
    QListIterator<QAction*> j(listActions);
    while (j.hasNext()) {
        QAction* pAction = j.next();
        if ( a_listChannelNames.contains(pAction->text()) ) {
            // this means the channel has been unselected by user or so
            disconnect(pAction, &QAction::triggered, this, &LMainWindow::openToolWindow);
            ui->menuTools->removeAction(pAction);
        }
    }
}

void LMainWindow::handleMainApiCmd(bool command)
{
    if (command) {
        // this is a run command
        if (m_channelState == stop) {
            m_pRunButton->click();
        }
    }
    else {
        // stop command
        m_pStopButton->click();
    }
}

void LMainWindow::handlePointApiCmd(bool command, QString pointName)
{
    if (command) {
        // select a point name if available
#if 0
        if (m_channelState == stop)
#endif
        {
            popSelectDataDlg();
            m_pSelectDataDlgWrapper->selectName(pointName);
            m_pSelectDataDlgWrapper->clickOk();
        }
    }
    else {
        // unselect a point name if available
#if 0
        if (m_channelState == stop)
#endif
        {
            popSelectDataDlg();
            m_pSelectDataDlgWrapper->unselectName(pointName);
            m_pSelectDataDlgWrapper->clickOk();
        }
    }
}

void LMainWindow::on_actionSelect_Data_triggered()
{
    // pop the dialog
    popSelectDataDlg();

    // execute the dialog
    m_pSelectDataDlgWrapper->execDialog();
}

void LMainWindow::initialize()
{
    //observers
    m_pWatchWindow  =   new LWatchWindow(this);
    m_pOscilloscope =   new LOscilloscope(this);
    m_pLog          =   new LSaveLog(this);
    connect(m_pWatchWindow, &LWatchWindow::sendScopeEnableState, this, &LMainWindow::receiveScopeEnableState);
    connect(m_pWatchWindow, &LWatchWindow::sendLogEnableState, this, &LMainWindow::receiveLogEnableState);
    connect(m_pWatchWindow, &LWatchWindow::sendCurveColor, this, &LMainWindow::receiveCurveColor);
    connect(m_pWatchWindow, &LWatchWindow::sendCurveSelected, this, &LMainWindow::receiveCurveSelected);
    connect(m_pWatchWindow, &LWatchWindow::sendMergeValueAxis, this, &LMainWindow::receiveMergeValueAxis);
    connect(m_pWatchWindow, &LWatchWindow::sendDemergeValueAxis, this, &LMainWindow::receiveDemergeValueAxis);


    qRegisterMetaType<LDataValue>("LDataValue");
    connect(m_pWatchWindow, &LWatchWindow::sendDataValue, LDataManager::instance(), &LDataManager::receiveDataValue);
    connect(m_pWatchWindow, &LWatchWindow::sendPullValueCmd, LDataManager::instance(), &LDataManager::receivePullValueCmd);


    LSettingsRelier* pWatchWindowSettingsRelier = m_pWatchWindow;
    LMainSettingsMgr::instance()->registerToSettingsMgr(pWatchWindowSettingsRelier);

    //dialogs
    m_pSelectDataDlgWrapper = new LSelectDataDlgWrapper;
    connect(m_pSelectDataDlgWrapper, &LSelectDataDlgWrapper::triggerAcceptSelection, this, &LMainWindow::acceptSelection);
    LSettingsRelier* pSelectDataSettingsRelier = m_pSelectDataDlgWrapper;
    LMainSettingsMgr::instance()->registerToSettingsMgr(pSelectDataSettingsRelier);

    m_pChannelMgrDlgWrapper = new LChannelMgrDlgWrapper;
    connect(m_pChannelMgrDlgWrapper, &LChannelMgrDlgWrapper::sendDeletedChannels, this, &LMainWindow::receiveDeletedChannels);
    LSettingsRelier* pChannelMgrSettingsRelier = m_pChannelMgrDlgWrapper;
    LMainSettingsMgr::instance()->registerToSettingsMgr(pChannelMgrSettingsRelier);

    // setting tab widget dialog framework
    m_pSettingTabWidget = new LSettingsTabDialog(this);

    // tool bar
    createToolBar();

    // menu
    createMenu();

    // script related
    m_pSMainWindow = new SMainWindow();
    connect(m_pSMainWindow, &SMainWindow::sendMainCmd, this, &LMainWindow::handleMainApiCmd);
    connect(m_pSMainWindow, &SMainWindow::sendPointCmd, this, &LMainWindow::handlePointApiCmd);
    connect(m_pSMainWindow, &SMainWindow::sendDataValue, LDataManager::instance(), &LDataManager::receiveDataValue);
    connect(m_pSMainWindow, &SMainWindow::sendPullValueCmd, LDataManager::instance(), &LDataManager::receivePullValueCmd);
    connect(m_pSMainWindow, &SMainWindow::sendCurveEnable, m_pWatchWindow, &LWatchWindow::receiveScopeEnableState);
    connect(m_pSMainWindow, &SMainWindow::sendCurveColor, m_pWatchWindow, &LWatchWindow::receiveCurveColor);
    connect(m_pSMainWindow, &SMainWindow::sendLogEnable, m_pWatchWindow, &LWatchWindow::receiveLogEnableState);

    // read history settings
    LMainSettingsMgr::instance()->readSettingsFile();

}

void LMainWindow::createToolBar()
{
    m_channelState = stop;

    //tool buttons
    m_pStopButton = new QToolButton(this);
    m_pStopButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "stop-red-icon.png")));
    m_pStopButton->setToolTip(tr("Click to stop all selected channels and release resourse"));
    ui->toolBar->addWidget(m_pStopButton);
    connect(m_pStopButton, &QToolButton::clicked, this, &LMainWindow::stopRunAllChannels);
    m_pStopButton->setDisabled(true);

    m_pRunButton = new QToolButton(this);
    m_pRunButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "start-icon.png")));
    m_pRunButton->setToolTip(tr("Click to run all selected channels"));
    ui->toolBar->addWidget(m_pRunButton);
    connect(m_pRunButton, &QToolButton::clicked, this, &LMainWindow::playPauseAllChannels);

    m_pConfigButton = new QToolButton(this);
    m_pConfigButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "Settings-icon.png")));
    m_pConfigButton->setToolTip(tr("Click to modify channel settings"));
    ui->toolBar->addWidget(m_pConfigButton);
    connect(m_pConfigButton, &QToolButton::clicked, this, &LMainWindow::openAllSettingsWindow);

    QToolButton* pConsoleButton = new QToolButton(this);
    pConsoleButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "Apps-konsole-icon.png")));
    pConsoleButton->setToolTip(tr("Click to open Script"));
    ui->toolBar->addWidget(pConsoleButton);
    connect(pConsoleButton, &QToolButton::clicked, this, &LMainWindow::openScriptIde);

    QToolButton* pPlaybackButton = new QToolButton(this);
    pPlaybackButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "log_replay.png")));
    pPlaybackButton->setToolTip(tr("Click to open Playback Window"));
    ui->toolBar->addWidget(pPlaybackButton);
    connect(pPlaybackButton, &QToolButton::clicked, this, &LMainWindow::openPlaybackWindow);

    QToolButton* pControlPageButton = new QToolButton(this);
    pControlPageButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "controlpage.png")));
    pControlPageButton->setToolTip(tr("Click to load Control Page"));
    ui->toolBar->addWidget(pControlPageButton);
    connect(pControlPageButton, &QToolButton::clicked, this, &LMainWindow::openControlPage);
}

void LMainWindow::createMenu()
{
    // create window menu
    QDockWidget* pWatchWindow = m_pWatchWindow;
    pWatchWindow->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::LeftDockWidgetArea, pWatchWindow);
    ui->menuWindows->addAction(pWatchWindow->toggleViewAction());

    QDockWidget* pOscilloscope = m_pOscilloscope;
    pOscilloscope->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, pOscilloscope);
    ui->menuWindows->addAction(pOscilloscope->toggleViewAction());

    QDockWidget* pLog = m_pLog;
    pLog->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, pLog);
    ui->menuWindows->addAction(pLog->toggleViewAction());
}

void LMainWindow::popSelectDataDlg()
{
    m_pSelectDataDlgWrapper->createDialog(this);

    LDataManager::instance()->fetchAllAvailableDataNames();

    m_pSelectDataDlgWrapper->setDataNameCollection(LDataManager::instance()->getDataNameCollection());

    m_pSelectDataDlgWrapper->loadHistory();
}

void LMainWindow::popChannelMgrDlg()
{
    m_pChannelMgrDlgWrapper->createDialog(this);

    m_pChannelMgrDlgWrapper->loadHistory();
}

void LMainWindow::createSettingTabWidget()
{
    QListIterator<QString> i(LChannelMgr::instance()->getChannelList());
    while(i.hasNext()) {
        QString strChannelName = i.next();
        QWidget* pSettingsWidget = LChannelMgr::instance()->getSettingsWidget(strChannelName);
        if (pSettingsWidget != nullptr) {
            m_pSettingTabWidget->addTab(pSettingsWidget, strChannelName);
        }
    }

    m_pSettingTabWidget->hide();
}

void LMainWindow::addToolWidget()
{
    // build a list of tool menu actions that already exists
    QList<QString> listActionTexts;
    QListIterator<QAction*> actionIter(ui->menuTools->actions());
    while (actionIter.hasNext()) {
        QAction* pAction = actionIter.next();
        listActionTexts.append(pAction->text());
    }

    QListIterator<QString> i(LChannelMgr::instance()->getChannelList());
    while(i.hasNext()) {
        QString strChannelName = i.next();
        if (!listActionTexts.contains(strChannelName)) {
            // if this channel tool widget has not been added yet
            QWidget* pSettingsWidget = LChannelMgr::instance()->getToolWidget(strChannelName);
            if (pSettingsWidget != nullptr) {
                // add tool menu
                pSettingsWidget->setParent(nullptr);
                QAction* pAction = ui->menuTools->addAction(strChannelName);
                connect(pAction, &QAction::triggered, this, &LMainWindow::openToolWindow);
            }
        }
    }
}

void LMainWindow::readWindowsSettings()
{
    /// \note this will force to use ini format to save settings of windows
    ///         no matter OS we are using
    ///         it will still work under ubuntu
    QSettings settings(PATH_ACCESSER->getPath(ePathProject, "WindowsSettings.ini"), QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    resize(settings.value("size").toSize());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

void LMainWindow::writeWindowsSettings()
{
    QSettings settings(PATH_ACCESSER->getPath(ePathProject, "WindowsSettings.ini"), QSettings::IniFormat);
    settings.beginGroup ("MainWindow");
    settings.setValue("size", size());
    settings.setValue("state", saveState());
    settings.endGroup();
}

void LMainWindow::acceptSelection(QList<QString> a_listNames)
{
    LValueObserver* pWatchWindow = m_pWatchWindow;
    LAttributesObserver* pAttriobv = m_pWatchWindow;
    m_pWatchWindow->setDataList(a_listNames);

    LValueObserver* pScriptWindow = m_pSMainWindow;
    m_pSMainWindow->setDataList(a_listNames);

    QListIterator<QString> i(a_listNames);
    while(i.hasNext()) {
        QString strDataName = i.next();
        // Watch window always observe all the value
        LDataManager::instance()->attachValueObserver(strDataName, pWatchWindow);
        // Watch window always observe all the attributes
        LDataManager::instance()->attachAttriObserver(strDataName, pAttriobv);
        // Script system always observe all the value
        LDataManager::instance()->attachValueObserver(strDataName, pScriptWindow);
    }

    // Pull all the attributes at this time
    LChannelMgr::instance()->pullAttriFromAllChannels();

    // Set the list name to the auto-complete dict in code editor
    m_pSMainWindow->setAutoCompModelStrings(a_listNames);
}

void LMainWindow::on_actionManager_triggered()
{
    popChannelMgrDlg();

    m_pChannelMgrDlgWrapper->execDialog();

    // create settings tab widget
    createSettingTabWidget();

    // handle all the channel widgets of channels
    addToolWidget();
#if 0
    // apply the change of channel, create all available data names
    LDataManager::instance()->fetchAllAvailableDataNames();
    // set the list name to the auto-complete dict in code editor
    m_pSMainWindow->setAutoCompModelStrings(LDataManager::instance()->getDataNameCollection());
#endif
}

void LMainWindow::on_actionContents_triggered()
{
    if(m_pHelpWidget->isHidden()) {
        m_pHelpWidget->show();
    }
}

void LMainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About Lobster",
                       "This program is for real time debug.\n\nVersion " + c_strVersion + "\n\nPowered by Steve Cai & Rusk Tao.");
}

void LMainWindow::on_actionSave_Project_triggered()
{
    // Remember window size .etc information
    writeWindowsSettings();

    // WatchWindow's information is not set automatically
    LSettingsRelier* pWatchWindowSettingsRelier = m_pWatchWindow;
    pWatchWindowSettingsRelier->convertToSetting();

    // Write history information
    LMainSettingsMgr::instance()->writeSettingsFile();
    LPluginSettingsMgr::instance()->writeSettingsFile();

}
