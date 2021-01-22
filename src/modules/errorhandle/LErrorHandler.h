#ifndef LERRORHANDLER_H
#define LERRORHANDLER_H

#include <QObject>

class LErrorHandler : public QObject
{
    Q_OBJECT
public:
    static LErrorHandler* instance();

    static void destroy();

public slots:
    void handlePluginError(qint32, QString a_strDescription, QString a_strSource);

private:
    LErrorHandler();

private:
    static LErrorHandler  *s_pInstance;
};

#endif // LERRORHANDLER_H
