#include "LCsvHandler.h"

#include <QStringListIterator>

LCsvHandler::LCsvHandler(const QString &a_rstrFileUrl) :
    m_file(a_rstrFileUrl),
    m_textStream(&m_file)
{

}

bool LCsvHandler::openFile()
{
    bool res;
    if (!m_file.isOpen()) {
        if (m_file.open(QIODevice::WriteOnly) ) {
            res = true;
        }
        else {
            res = false;
        }
    }
    else {
        res = false;
    }
    return res;
}

void LCsvHandler::addNewLine(const QStringList &a_rstrLineContent)
{
    QStringListIterator lineContentIterator(a_rstrLineContent);
    while (lineContentIterator.hasNext())
    {
        m_textStream << lineContentIterator.next();
    }
    // insert the return string
    m_textStream << "\n";
}

bool LCsvHandler::saveFile()
{
    // Do nothing
    return true;
}

bool LCsvHandler::closeFile()
{
    m_file.close();
    return true;
}

bool LCsvHandler::saveAs(const QString &a_rstrNewFileName)
{
    return m_file.copy(a_rstrNewFileName);
}

void LCsvHandler::flush()
{
    m_textStream.flush();
}

bool LCsvHandler::appendFile()
{
    bool res;
    if (!m_file.isOpen()) {
        if (m_file.open(QIODevice::Append) ) {
            res = true;
        }
        else {
            res = false;
        }
    }
    else {
        res = false;
    }
    return res;
}

LCsvHandler::LCsvHandler()
{
}
