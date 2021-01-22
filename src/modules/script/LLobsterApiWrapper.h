#ifndef LLOBSTERAPIWRAPPER_H
#define LLOBSTERAPIWRAPPER_H

#include <QObject>
#include <QList>

#include "LMainApi.h"
#include "LSystemApi.h"
#include "LPointApi.h"

class LLobsterApiWrapper : public QObject
{
    Q_OBJECT
public:
    explicit LLobsterApiWrapper(QObject *parent = 0);
    ~LLobsterApiWrapper();

    QList<LApi*> getApiList() const;

    LPointApi* getPointApiObj();

    LMainApi* getMainApiObj();

    LSystemApi* getSystemApiObj();

signals:

public slots:

private:
    QList<LApi*> m_listApis;

    LMainApi*       m_pMainApi;
    LPointApi*      m_pPointApi;
    LSystemApi*     m_pSystemApi;
};

#endif // LLOBSTERAPIWRAPPER_H
