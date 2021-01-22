#ifndef LDATALOGHANGLER_H
#define LDATALOGHANGLER_H

#include <QString>

class LDataLogHangler
{
public:
    LDataLogHangler() {}
    virtual ~LDataLogHangler() {}

    virtual bool openFile() = 0;

    virtual bool saveFile() = 0;

    virtual bool closeFile() = 0;

    virtual void addNewLine(const QStringList& a_rstrLineContent) = 0;

    virtual void flush() = 0;

};

#endif // LDATALOGHANGLER_H
