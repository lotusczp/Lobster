#ifndef LAPI_H
#define LAPI_H

#include <QObject>

class LApi : public QObject
{
    Q_OBJECT
public:
    explicit LApi(QObject *parent = 0);

signals:
    void apiCallError(QString errorMsg);

public slots:

};

#endif // LAPI_H
