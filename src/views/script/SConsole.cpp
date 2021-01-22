#include "SConsole.h"
#include <QScrollBar>

#define REFRESH_TIME_ms   500

SConsole::SConsole(QWidget *parent) :
    QPlainTextEdit(parent)
{
    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(REFRESH_TIME_ms);
    m_pTimer->setSingleShot(true);
    connect(m_pTimer, &QTimer::timeout, this, &SConsole::refresh);
    m_pTimer->start();
}

SConsole::~SConsole()
{
    delete m_pTimer;
}

void SConsole::addText(const QString &a_rstrText)
{
    m_strText += a_rstrText;
    if(!m_pTimer->isActive()) {
        m_pTimer->start();
    }
}

void SConsole::clearText()
{
    m_strText.clear();
    this->clear();
}

void SConsole::refresh()
{
    this->appendPlainText(m_strText);
    m_strText.clear();
}


