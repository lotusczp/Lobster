#ifndef LMAINAPI_H
#define LMAINAPI_H

#include "LApi.h"

class LMainApi : public LApi
{
    Q_OBJECT
public:
    explicit LMainApi(QObject *parent = 0);

signals:
    /// \note comand=true for start, false for stop
    void sendCmd(bool command);

public slots:
    void startTrans();

    void stopTrans();
};

#endif // LMAINAPI_H
