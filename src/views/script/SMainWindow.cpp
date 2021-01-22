#include "SMainWindow.h"
#include "ui_SMainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>
#include <QStringListModel>
#include <QScrollBar>

#include "LSystemApi.h"
#include "LMainApi.h"
#include "LPointApi.h"
#include "LUniqueResource.h"

static const QString c_strTittle = "Lobster Script IDE";


SMainWindow::SMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SMainWindow)
{
    ui->setupUi(this);

    setupEditor();

    setupConsole();

    setupSplitter();

    setupToolbar();

    setWindowTitle(c_strTittle);

    // audio related
    m_pSound = new QSound(PATH_ACCESSER->getPath(ePathAudio, "beep.wav"));

    m_pTextToSpeech = new QTextToSpeech(this);

    m_pScriptEval = nullptr;

    m_pLobsterAPI = new LLobsterApiWrapper(this);
    // the system api use this signal to send content to console window
    connect(m_pLobsterAPI->getSystemApiObj(), &LSystemApi::sendMsg, this, &SMainWindow::addText2Console);
    connect(m_pLobsterAPI->getSystemApiObj(), &LSystemApi::sendCmdBeep, this, &SMainWindow::handleSysApiBeepCmd);
    connect(m_pLobsterAPI->getSystemApiObj(), &LSystemApi::sendCmdSay, this, &SMainWindow::handleSysApiSayCmd);
    // the main api use this signal to send command to lobster main program
    connect(m_pLobsterAPI->getMainApiObj(), &LMainApi::sendCmd, this, &SMainWindow::handleMainApiCmd);
    // the point api use this signal to send command to lobster main program
    connect(m_pLobsterAPI->getPointApiObj(), &LPointApi::sendSubCmd, this, &SMainWindow::handlePointApiSubCmd);
    connect(m_pLobsterAPI->getPointApiObj(), &LPointApi::sendDataValue, this, &SMainWindow::receiveDataValue);
    connect(m_pLobsterAPI->getPointApiObj(), &LPointApi::sendCurveEnable, this, &SMainWindow::handlePointApiCurveEnableCmd);
    connect(m_pLobsterAPI->getPointApiObj(), &LPointApi::sendCurveColorCmd, this, &SMainWindow::handlePointApiCurveColorCmd);
    connect(m_pLobsterAPI->getPointApiObj(), &LPointApi::sendCurveGainCmd, this, &SMainWindow::handlePointApiCurveGainCmd);
    connect(m_pLobsterAPI->getPointApiObj(), &LPointApi::sendCurveOffsetCmd, this, &SMainWindow::handlePointApiCurveOffsetCmd);
    connect(m_pLobsterAPI->getPointApiObj(), &LPointApi::sendPullValueCmd, this, &SMainWindow::sendPullValueCmd);
    connect(m_pLobsterAPI->getPointApiObj(), &LPointApi::sendLogEnable, this, &SMainWindow::sendLogEnable);

    // all the api stuff use this signal to report calling error
    QListIterator<LApi*> it(m_pLobsterAPI->getApiList());
    while (it.hasNext()) {
        LApi* pApi = it.next();
        connect(pApi, &LApi::apiCallError, this, &SMainWindow::handleApiCallError);
    }
}

SMainWindow::~SMainWindow()
{
    delete m_pSound;
    delete m_pTextToSpeech;

    delete ui;
}

void SMainWindow::setAutoCompModelStrings(const QList<QString> &a_rModelStringlist)
{
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif

    QStringList words(a_rModelStringlist);

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    // set the string to the auto-completion list
    m_pCompleter->setModel(new QStringListModel(words, m_pCompleter));
}

void SMainWindow::setDataList(const QList<QString> &a_rDataList)
{
    m_pLobsterAPI->getPointApiObj()->setDataList(a_rDataList);
}

void SMainWindow::updateValue(const QString &a_rDataName, const LDataValue &a_rDataValue)
{
    m_pLobsterAPI->getPointApiObj()->updateDataValue(a_rDataName, a_rDataValue);
}

void SMainWindow::start()
{
    // nothing to do here
}

void SMainWindow::stop()
{
    // nothing to do here
}

void SMainWindow::addText2Console(QString text)
{
    ui->console->addText(text);
}

void SMainWindow::handleApiCallError(QString ErrorMsg)
{
    // show the error message
    addText2Console(ErrorMsg+"\n");
    // stop the evaluation
    stopScript();
}

void SMainWindow::handleMainApiCmd(bool command)
{
    emit sendMainCmd(command);
}

void SMainWindow::handlePointApiSubCmd(bool command, QString pointName)
{
    emit sendPointCmd(command, pointName);
}

void SMainWindow::handleSysApiBeepCmd(int loops)
{
    m_pSound->setLoops(loops);
    m_pSound->play();
}

void SMainWindow::handleSysApiSayCmd(QString content)
{
    m_pTextToSpeech->say(content);
}

void SMainWindow::receiveDataValue(QString a_strDataName, LDataValue a_dDataValue)
{
    emit sendDataValue(a_strDataName, a_dDataValue);
}

void SMainWindow::handlePointApiCurveEnableCmd(QString a_strDataName, bool a_bIsEnable)
{
    emit sendCurveEnable(a_strDataName, a_bIsEnable);
}

void SMainWindow::handlePointApiCurveColorCmd(QString a_strDataName, QColor a_curveColor)
{
    emit sendCurveColor(a_strDataName, a_curveColor);
}

void SMainWindow::handlePointApiCurveGainCmd(QString a_strDataName, float a_fCurveGain)
{
    emit sendCurveGain(a_strDataName, a_fCurveGain);
}

void SMainWindow::handlePointApiCurveOffsetCmd(QString a_strDataName, float a_fCurveOffset)
{
    emit sendCurveOffset(a_strDataName, a_fCurveOffset);
}

void SMainWindow::closeEvent(QCloseEvent *event)
{
    // stop if still running
    if ( (m_pScriptEval) && (!m_pScriptEval->isFinished()) ) {
        m_pScriptEval->stopEvaluate();
    }


    // try to save the script
    if (m_strScript != ui->codeEditor->toPlainText()) {
        if (QMessageBox::question(this, "Do you want to save it", "The script has been modified. Do you want to save it?") == QMessageBox::Yes) {
            saveScript();
        }
        else {
            // discard the changes
            ui->codeEditor->setPlainText(m_strScript);
        }
    }

    // accept close event
    event->accept();
}

void SMainWindow::setupEditor()
{
    // setup auto-completion
    m_pCompleter = new QCompleter(this);
    m_pCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_pCompleter->setWrapAround(false);
    ui->codeEditor->setCompleter(m_pCompleter);
}

void SMainWindow::setupConsole()
{
    ui->console->setReadOnly(true);
}

void SMainWindow::setupSplitter()
{
    QList<int> sizeList;
    sizeList.append(400);
    sizeList.append(200);
    ui->splitter->setSizes(sizeList);
}

void SMainWindow::setupToolbar()
{
    // tool buttons

    QToolButton* pNewButton = new QToolButton(this);
    pNewButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "new-file-icon.png")));
    pNewButton->setToolTip(tr("Click to new a script"));
    ui->toolBar->addWidget(pNewButton);
    connect(pNewButton, &QToolButton::clicked, this, &SMainWindow::newScript);

    QToolButton* pSaveButton = new QToolButton(this);
    pSaveButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "save-icon.png")));
    pSaveButton->setToolTip(tr("Click to save the current script"));
    ui->toolBar->addWidget(pSaveButton);
    connect(pSaveButton, &QToolButton::clicked, this, &SMainWindow::saveScript);

    QToolButton* pOpenButton = new QToolButton(this);
    pOpenButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "folder-open-icon.png")));
    pOpenButton->setToolTip(tr("Click to open a script"));
    ui->toolBar->addWidget(pOpenButton);
    connect(pOpenButton, &QToolButton::clicked, this, &SMainWindow::openScript);

    QToolButton* pRefreshButton = new QToolButton(this);
    pRefreshButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "refresh-icon.png")));
    pRefreshButton->setToolTip(tr("Click to refresh the current script"));
    ui->toolBar->addWidget(pRefreshButton);
    connect(pRefreshButton, &QToolButton::clicked, this, &SMainWindow::refreshScript);

    m_pStopScriptButton = new QToolButton(this);
    m_pStopScriptButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "stop-red-icon.png")));
    m_pStopScriptButton->setToolTip(tr("Click to stop running current script"));
    ui->toolBar->addWidget(m_pStopScriptButton);
    connect(m_pStopScriptButton, &QToolButton::clicked, this, &SMainWindow::stopScript);
    m_pStopScriptButton->setEnabled(false);

    m_pRunScriptButton = new QToolButton(this);
    m_pRunScriptButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "script_start.png")));
    m_pRunScriptButton->setToolTip(tr("Click to run current script"));
    ui->toolBar->addWidget(m_pRunScriptButton);
    connect(m_pRunScriptButton, &QToolButton::clicked, this, &SMainWindow::runScript);

    m_pDebugScriptButton = new QToolButton(this);
    m_pDebugScriptButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "start_debug.png")));
    m_pDebugScriptButton->setToolTip(tr("Click to debug current script"));
    ui->toolBar->addWidget(m_pDebugScriptButton);
    connect(m_pDebugScriptButton, &QToolButton::clicked, this, &SMainWindow::debugScript);

    QToolButton* pCleanButton = new QToolButton(this);
    pCleanButton->setIcon(QIcon(PATH_ACCESSER->getPath(ePathImage, "clear-icon.png")));
    pCleanButton->setToolTip(tr("Click to clean the console"));
    ui->toolBar->addWidget(pCleanButton);
    connect(pCleanButton, &QToolButton::clicked, this, &SMainWindow::cleanConsole);
}

void SMainWindow::setupScriptEval()
{
    if (m_pScriptEval == NULL) {
        m_pScriptEval = new LScriptEvalWrapper(this);
        connect(m_pScriptEval, &LScriptEvalWrapper::evalFinished, this, &SMainWindow::scriptStopped);
        connect(m_pScriptEval, &LScriptEvalWrapper::reportEvalError, this, &SMainWindow::addText2Console);
        // set lobster api object to the evaluator
        m_pScriptEval->setLobsterAPI(m_pLobsterAPI);
    }
}

void SMainWindow::editTittle()
{
    if (m_strScriptPath == "") {
        setWindowTitle(c_strTittle);
    }
    else {
        setWindowTitle(c_strTittle + " : " + m_strScriptPath);
    }
}

void SMainWindow::newScript()
{
    // try to save the script
    if (m_strScript != ui->codeEditor->toPlainText()) {
        if (QMessageBox::question(this, "Do you want to save it", "The script has been modified. Do you want to save it?") == QMessageBox::Yes) {
            saveScript();
        }
    }

    ui->codeEditor->clear();
    m_strScriptPath = "";
    m_strScript = "";

    editTittle();
}

void SMainWindow::saveScript()
{
    if (m_strScript != ui->codeEditor->toPlainText()) {
        QString fileName;
        if (m_strScriptPath.isEmpty()) {
            fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save ECMA script"),
                                                    PATH_ACCESSER->getPath(ePathScript),
                                                    tr("ECMA script (*.js)"));
        }
        else {
            fileName = m_strScriptPath;
        }

        if(!fileName.isEmpty())
        {
            QFile file(fileName);
            if(file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&file);
                out << ui->codeEditor->toPlainText();
                out.flush();
                file.close();
                // remember the current file path
                m_strScript = ui->codeEditor->toPlainText();
                m_strScriptPath = fileName;
                editTittle();
            }
            else
            {
                QMessageBox::warning(0,QObject::tr("Warning"), "can not write this files.\nYou may have permission problems");
            }
        }
    }
}

void SMainWindow::openScript()
{
    // try to save the script
    if (m_strScript != ui->codeEditor->toPlainText()) {
        if (QMessageBox::question(this, "Do you want to save it", "The script has been modified. Do you want to save it?") == QMessageBox::Yes) {
            saveScript();
        }
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open ECMA script"),
                                                    PATH_ACCESSER->getPath(ePathScript),
                                                    tr("ECMA script (*.js)"));

    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            ui->codeEditor->setPlainText(in.readAll());
            file.close();
            m_strScript = ui->codeEditor->toPlainText();
            m_strScriptPath = fileName;
            editTittle();
        }
        else
        {
            QMessageBox::warning(0,QObject::tr("Warning"), "can not open this file.\nYou may have permission problems");
        }
    }
}

void SMainWindow::refreshScript()
{
    QString fileName = m_strScriptPath;
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            ui->codeEditor->setPlainText(in.readAll());
            file.close();
            m_strScript = ui->codeEditor->toPlainText();
        }
        else
        {
            QMessageBox::warning(0,QObject::tr("Warning"), "can not open this file.\nYou may have permission problems");
        }
    }
}

void SMainWindow::runScript()
{   
    // try to save it if it's not saved
    if ( (m_strScriptPath.isEmpty()) || (m_strScript != ui->codeEditor->toPlainText()) ) {
        saveScript();
    }

    if (!m_strScriptPath.isEmpty()) {
        // disable the run button and enable the stop button
        m_pRunScriptButton->setEnabled(false);
        m_pDebugScriptButton->setEnabled(false);
        m_pStopScriptButton->setEnabled(true);

        // show start information in console
        addText2Console("Script running started at\n" + QDateTime::currentDateTime().toString() + "\n");
        // create script evaluator
        setupScriptEval();
        // notice that script evaluator must be created before this
        m_pScriptEval->startEvaluate(m_strScript, m_strScriptPath);
    }
}

void SMainWindow::debugScript()
{
    // try to save it if it's not saved
    if ( (m_strScriptPath.isEmpty()) || (m_strScript != ui->codeEditor->toPlainText()) ) {
        saveScript();
    }

    if (!m_strScriptPath.isEmpty()) {
        // disable the run button and enable the stop button
        m_pRunScriptButton->setEnabled(false);
        m_pDebugScriptButton->setEnabled(false);
        m_pStopScriptButton->setEnabled(true);

        // show start information in console
        addText2Console("Script debugging started at\n" + QDateTime::currentDateTime().toString() + "\n");
        // create script evaluator
        setupScriptEval();
        // notice that script evaluator must be created before this
        m_pScriptEval->debugEvaluate(m_strScript, m_strScriptPath);
    }
}

void SMainWindow::stopScript()
{
    if (m_pScriptEval) {
        m_pScriptEval->stopEvaluate();
    }
    addText2Console("Abort evaluation.\n");
}

void SMainWindow::scriptStopped()
{
    if (m_pScriptEval) {
        delete m_pScriptEval, m_pScriptEval = NULL;
    }

    // enable the run button and disable the stop button
    m_pRunScriptButton->setEnabled(true);
    m_pDebugScriptButton->setEnabled(true);
    m_pStopScriptButton->setEnabled(false);

    // show stop information in console
    addText2Console("\nScript running stopped at\n" + QDateTime::currentDateTime().toString() + "\n\n");
}

void SMainWindow::cleanConsole()
{
    ui->console->clearText();
}

void SMainWindow::on_actionNew_triggered()
{
    newScript();
}

void SMainWindow::on_actionSave_triggered()
{
    saveScript();
}

void SMainWindow::on_actionOpen_triggered()
{
    openScript();
}

void SMainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save ECMA script"),
                                                    PATH_ACCESSER->getPath(ePathScript),
                                                    tr("ECMA script (*.js)"));

    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << ui->codeEditor->toPlainText();
            out.flush();
            file.close();
            // remember the current file path
            m_strScript = ui->codeEditor->toPlainText();
            m_strScriptPath = fileName;
            editTittle();
        }
        else
        {
            QMessageBox::warning(0,QObject::tr("Warning"), "can not write this files.\nYou may have permission problems");
        }
    }
}
