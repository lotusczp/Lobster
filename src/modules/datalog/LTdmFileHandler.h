#ifndef LTDMFILEHANDLER_H
#define LTDMFILEHANDLER_H

#include <QMap>
#include "LDataLogHangler.h"
#include "LTdmDllWrapper.h"

class LTdmFileHandler : public LDataLogHangler
{
public:
    LTdmFileHandler();

    bool openFile();

    bool saveFile();

    bool closeFile();

    void addNewLine(const QStringList& a_rstrLineContent);

    void flush();

private:
    DDCFileHandle                           m_fileHandle;
    QMap<QString, DDCChannelGroupHandle>    m_mapGroupHandles;
};

#endif // LTDMFILEHANDLER_H
