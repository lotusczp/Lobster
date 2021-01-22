#ifndef SMAINWINDOW_H
#define SMAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QCompleter>
#include <QSound>
#include <QTextToSpeech>

#include "LLobsterApiWrapper.h"
#include "LScriptEvalWrapper.h"

#include "LValueObserver.h"

namespace Ui {
class SMainWindow;
}

class SMainWindow : public QMainWindow, public LValueObserver
{
    Q_OBJECT

public:
    explicit SMainWindow(QWidget *parent = 0);
    ~SMainWindow();

    // set auto completion reference string list
    void setAutoCompModelStrings(const QList<QString>& a_rModelStringlist);

    void setDataList(const QList<QString>& a_rDataList);

    // inherit from LObserver
    virtual void updateValue(const QString& a_rDataName, const LDataValue& a_rDataValue) Q_DECL_OVERRIDE;
    virtual void start() Q_DECL_OVERRIDE;
    virtual void stop() Q_DECL_OVERRIDE;

public slots:
    void addText2Console(QString text);

    void handleApiCallError(QString ErrorMsg);

    void handleMainApiCmd(bool command);

    void handlePointApiSubCmd(bool command, QString pointName);

    void handleSysApiBeepCmd(int loops);

    void handleSysApiSayCmd(QString content);

    void receiveDataValue(QString a_strDataName, LDataValue a_dDataValue);

    void handlePointApiCurveEnableCmd(QString a_strDataName, bool a_bIsEnable);

    void handlePointApiCurveColorCmd(QString a_strDataName, QColor a_curveColor);

    void handlePointApiCurveGainCmd(QString a_strDataName, float a_fCurveGain);

    void handlePointApiCurveOffsetCmd(QString a_strDataName, float a_fCurveOffset);

signals:
    /// \note comand=true for start, false for stop
    void sendMainCmd(bool command);
    /// \note command=true for subscribe, false for unsubscribe
    void sendPointCmd(bool command, QString pointName);

    void sendDataValue(QString a_strDataName, LDataValue a_dDataValue);
    void sendPullValueCmd(QString a_strDataName);

    void sendCurveEnable(QString a_strDataName, bool a_bIsEnable);

    void sendCurveColor(QString a_strDataName, QColor a_curveColor);

    void sendCurveGain(QString a_strDataName, float a_fCurveGain);

    void sendCurveOffset(QString a_strDataName, float a_fCurveOffset);

    void sendLogEnable(QString a_strDataName, bool a_bIsEnable);


protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    void setupEditor();
    void setupConsole();
    void setupSplitter();
    void setupToolbar();

    void setupScriptEval();

    void editTittle();

private slots:
    void newScript();
    void saveScript();
    void openScript();
    void refreshScript();
    void runScript();
    void debugScript();
    void stopScript();
    void scriptStopped();

    void cleanConsole();

    void on_actionNew_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_As_triggered();

private:
    Ui::SMainWindow *ui;

    // editor related
    QCompleter *m_pCompleter;

    // buttons
    QToolButton* m_pRunScriptButton;

    QToolButton* m_pDebugScriptButton;

    QToolButton* m_pStopScriptButton;

    // script
    LScriptEvalWrapper* m_pScriptEval;

    LLobsterApiWrapper* m_pLobsterAPI;

    // codes
    QString m_strScriptPath;

    QString m_strScript;

    // audio support
    QSound          *m_pSound;

    QTextToSpeech   *m_pTextToSpeech;
};

#endif // SMAINWINDOW_H
