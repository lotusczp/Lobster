#include "LSystemApi.h"

#include <QTimer>
#include <QSound>
#include <QDateTime>
#include <QAction>

LSystemApi::LSystemApi(QObject *parent) :
    LApi(parent)
{
    m_pEventloop = NULL;
    m_pDebugger = NULL;
}

void LSystemApi::setDebugger(QScriptEngineDebugger *a_pDebugger)
{
    m_pDebugger = a_pDebugger;
}

void LSystemApi::wait(QScriptValue a_milliSeconds)
{
    if (a_milliSeconds.isNumber()) {
        if (m_pEventloop == NULL) {
            m_pEventloop = new QEventLoop;
        }
        QTimer::singleShot(a_milliSeconds.toInt32(), m_pEventloop, SLOT(quit()));
        m_pEventloop->exec();
        delete m_pEventloop, m_pEventloop = NULL;
    }
    else {
        emit apiCallError("Error: The Function \"wait(milliSeconds)\" must take a number as argument.");
    }
}

void LSystemApi::consoleWrite(QScriptValue string)
{
    emit sendMsg(string.toString()+"\n");
}

void LSystemApi::beep(QScriptValue times)
{
    int loops = 0;
    if (times.isNumber()) {
        loops = (int)times.toNumber();
    }
    emit sendCmdBeep(loops);
}

void LSystemApi::say(QScriptValue content)
{
    emit sendCmdSay(content.toString());
}

QScriptValue LSystemApi::absTime()
{
    return QScriptValue(QDateTime::currentMSecsSinceEpoch()/1000.0);
}

void LSystemApi::pause()
{
    if (m_pDebugger != NULL) {
        m_pDebugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
    }
}

void LSystemApi::stopWaiting()
{
    if (m_pEventloop != NULL) {
        m_pEventloop->quit();
    }
}
