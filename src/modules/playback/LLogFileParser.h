#ifndef LLOGFILEPARSER_H
#define LLOGFILEPARSER_H

#include <QObject>
#include <QVector>
#include <QStringList>

#include "LDataValue.h"

class LLogFileParser : public QObject
{
    Q_OBJECT
public:
    explicit LLogFileParser(QObject *parent = nullptr);

signals:
    void sendDataValue(QString name, LDataValue value);

    void sendErrors(QString errorMessage);

    void sendNames(QStringList valueNames);

    void sendProcessPercent(int percent);

    void sendProcessFinished();

public slots:
    void parseFile(QString filePath);
};

#endif // LLOGFILEPARSER_H
