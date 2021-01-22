#ifndef LCSVHANDLER_H
#define LCSVHANDLER_H

#include <QFile>
#include <QString>
#include <QTextStream>
#include "LDataLogHangler.h"

class LCsvHandler : public LDataLogHangler
{
public:
    LCsvHandler(const QString& a_rstrFileUrl);

    bool openFile();

    void addNewLine(const QStringList& a_rstrLineContent);

    bool saveFile();

    bool closeFile();

    bool saveAs(const QString& a_rstrNewFileName);

    void flush();

    bool appendFile();

protected:
    LCsvHandler();

protected:
    //!< the csv file
    QFile m_file;

    //!< the text stream operator
    QTextStream m_textStream;

};

#endif // LCSVHANDLER_H
