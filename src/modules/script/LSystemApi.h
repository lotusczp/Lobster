#ifndef LSYSTEMAPI_H
#define LSYSTEMAPI_H

#include "LApi.h"
#include <QScriptValue>
#include <QEventLoop>
#include <QScriptEngineDebugger>

class LSystemApi : public LApi
{
    Q_OBJECT
public:
    explicit LSystemApi(QObject *parent = 0);

    void setDebugger(QScriptEngineDebugger* a_pDebugger);

signals:
    void sendMsg(QString msg);

    void sendCmdBeep(int loops);

    void sendCmdSay(QString content);

public slots:
    // Apis for js
    void wait(QScriptValue a_milliSeconds);

    void consoleWrite(QScriptValue string);

    void beep(QScriptValue times);

    void say(QScriptValue content);

    QScriptValue absTime();

    void pause();

    // slots for lobster
    void stopWaiting();

private:
    QEventLoop* m_pEventloop;

    QScriptEngineDebugger* m_pDebugger;
};

#endif // LSYSTEMAPI_H
