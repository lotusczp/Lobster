#ifndef LSCRIPTEVALWRAPPER_H
#define LSCRIPTEVALWRAPPER_H

#include <QString>
#include <QScriptEngine>
#include <QScriptEngineDebugger>

#include "LLobsterApiWrapper.h"

class LScriptEvalWrapper : public QObject
{
    Q_OBJECT
public:
    explicit LScriptEvalWrapper(QObject *parent = 0);

    void startEvaluate(const QString& a_rstrCodes, const QString& a_rstrFileName);

    void debugEvaluate(const QString& a_rstrCodes, const QString& a_rstrFileName);

    void stopEvaluate();

    void setLobsterAPI(LLobsterApiWrapper* a_pLobsterObject);

    bool isFinished() const;

signals:
    void reportEvalError(QString errMsg);

    void evalFinished();

    void shutDownWaiting();

public slots:

private:
    bool checkSyntax(const QString& a_rstrCodes);

private:
    QScriptEngine m_engine;

    QScriptEngineDebugger m_debugger;

    QString m_strCodes;

    QString m_strFileName;
};

#endif // LSCRIPTEVALWRAPPER_H
