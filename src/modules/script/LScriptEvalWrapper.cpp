#include "LScriptEvalWrapper.h"
#include <QAction>
#include <QTextStream>
#include <QMainWindow>
#include <QFile>
#include "LUniqueResource.h"

QList<QString> s_listFilesInclude;

QScriptValue FunInclude(QScriptContext *ctx, QScriptEngine *eng)
{
    QString fileName = PATH_ACCESSER->getPath(ePathScript) + ctx->argument(0).toString();
    if (!s_listFilesInclude.contains(fileName)) {
        s_listFilesInclude.append(fileName);
        QString contents;
        if(!fileName.isEmpty())
        {
            QFile file(fileName);
            if(file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&file);
                contents = in.readAll();
                file.close();
            }
        }
        ctx->setActivationObject(ctx->parentContext()->activationObject());
        ctx->setThisObject(ctx->parentContext()->thisObject());
        return eng->evaluate(contents, fileName);
    }
    else {
        return QScriptValue(QScriptValue::NullValue);
    }
}

QScriptValue FuncBacktrace(QScriptContext *ctx, QScriptEngine * /*eng*/)
{
    QStringList lstStack = ctx->backtrace();
    QString strStack = lstStack.join("\n");
    return QScriptValue(strStack);
}

LScriptEvalWrapper::LScriptEvalWrapper(QObject *parent) :
    QObject(parent),
    m_debugger(this),
    m_engine(this)
{
    // add the built-in "include" function
    QScriptValue func = m_engine.newFunction(FunInclude);
    m_engine.globalObject().setProperty("include", func);

    // Add the built-in "backtrack" function
    func = m_engine.newFunction(FuncBacktrace);
    m_engine.globalObject().setProperty("backtrace", func);
}

void LScriptEvalWrapper::startEvaluate(const QString &a_rstrCodes, const QString &a_rstrFileName)
{
    // get codes and file name
    m_strCodes = a_rstrCodes;
    m_strFileName = a_rstrFileName;
    // check syntax before running evaluating
    bool res = checkSyntax(a_rstrCodes);
    // if the syntax check pass
    if (res) {
        // no need debugger
        m_debugger.detach();
        // Main program events processing shouldn't block
        m_engine.setProcessEventsInterval(50);
        // start evaluation
        m_engine.evaluate(m_strCodes, m_strFileName);
        // whether has exceptions
        if (m_engine.hasUncaughtException()) {
            // Handle the exception first
            m_engine.globalObject().property("exceptionHandler").call();
            // Report
            emit reportEvalError(m_engine.uncaughtException().toString()+"\n"+"See Call stack as below:\n");
            QString strStack = m_engine.uncaughtExceptionBacktrace().join("\n");
            emit reportEvalError(strStack + "\n");
        }
        // clear include enviroment
        s_listFilesInclude.clear();
        // imply evalution finished
        emit evalFinished();
    }
}

void LScriptEvalWrapper::debugEvaluate(const QString &a_rstrCodes, const QString &a_rstrFileName)
{
    // get codes and file name
    m_strCodes = a_rstrCodes;
    m_strFileName = a_rstrFileName;
    // check syntax before running evaluating
    bool res = checkSyntax(a_rstrCodes);
    // if the syntax check pass
    if (res) {
        // set debugger
        m_debugger.detach();// detach if already has it
        m_debugger.attachTo(&m_engine);
        // stop debug on first line
        m_debugger.action(QScriptEngineDebugger::InterruptAction)->trigger();
        // Main program events processing shouldn't block
        m_engine.setProcessEventsInterval(50);
        // start evaluation
        m_engine.evaluate(m_strCodes, m_strFileName);
        /// \note notice that the exception will be caught by debugger
        ///       unable to use "hasUncaughtException()" here.
        // clear include enviroment
        s_listFilesInclude.clear();
        // imply evalution finished
        emit evalFinished();
    }
}

void LScriptEvalWrapper::stopEvaluate()
{
    QMainWindow* pDebugWindow = m_debugger.standardWindow();
    if (!pDebugWindow->isHidden()) {
        pDebugWindow->close();
    }
    m_engine.abortEvaluation();

    emit shutDownWaiting();
}

void LScriptEvalWrapper::setLobsterAPI(LLobsterApiWrapper *a_pLobsterObject)
{
    QScriptValue mainApiValue = m_engine.newQObject(a_pLobsterObject->getMainApiObj());
    m_engine.globalObject().setProperty("lbMain", mainApiValue);

    QScriptValue PointApiValue = m_engine.newQObject(a_pLobsterObject->getPointApiObj());
    m_engine.globalObject().setProperty("lbPoint", PointApiValue);

    LSystemApi* pSystemApi = a_pLobsterObject->getSystemApiObj();
    QScriptValue SystemApiValue = m_engine.newQObject(pSystemApi);
    m_engine.globalObject().setProperty("lbSystem", SystemApiValue);
    pSystemApi->setDebugger(&m_debugger);
    connect(this, &LScriptEvalWrapper::shutDownWaiting, pSystemApi, &LSystemApi::stopWaiting);
}

bool LScriptEvalWrapper::isFinished() const
{
    return (!m_engine.isEvaluating());
}

bool LScriptEvalWrapper::checkSyntax(const QString &a_rstrCodes)
{
    // check syntax before running evaluating
    QScriptSyntaxCheckResult res = m_engine.checkSyntax(a_rstrCodes);
    if (res.state() != QScriptSyntaxCheckResult::Valid) {
        QString strErrorMsg = "Error: " + res.errorMessage() +
                " in line " + QString::number(res.errorLineNumber()) +
                " column " + QString::number(res.errorColumnNumber()) +
                "\n";
        emit reportEvalError(strErrorMsg);
        emit evalFinished();
        return false;
    }
    return true;
}
