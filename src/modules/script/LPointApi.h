#ifndef LPOINTAPI_H
#define LPOINTAPI_H

#include "LApi.h"
#include <QScriptValue>
#include "LDataValue.h"
#include <QMap>
#include <QColor>

class LPointApi : public LApi
{
    Q_OBJECT
public:
    explicit LPointApi(QObject *parent = 0);

    void setDataList(const QList<QString>& a_rDataList);

    void updateDataValue(const QString& a_rDataName, const LDataValue& a_rDataValue);

signals:
    /// \note command=true for subscribe, false for unsubscribe
    void sendSubCmd(bool command, QString pointName);

    void sendDataValue(QString a_strDataName, LDataValue a_dDataValue);

    void sendCurveEnable(QString a_strDataName, bool a_bIsEnable);

    void sendCurveColorCmd(QString a_strDataName, QColor a_curveColor);

    void sendCurveGainCmd(QString a_strDataName, float a_fCurveGain);

    void sendCurveOffsetCmd(QString a_strDataName, float a_fCurveOffset);

    void sendPullValueCmd(QString a_strDataName);

    void sendLogEnable(QString a_strDataName, bool a_bIsEnable);

public slots:
    void subscribe(QScriptValue a_dataName);

    void unsubscribe(QScriptValue a_dataName);

    QScriptValue getValue(QScriptValue a_dataName);

    void setValue(QScriptValue a_dataName, QScriptValue a_dataValue);

    /// \note boolString true for enable, false for disable
    void setCurveEnable(QScriptValue a_dataName, QScriptValue a_boolString);

    void setCurveColor(QScriptValue a_dataName, QScriptValue a_colorString);

    void setCurveGain(QScriptValue a_dataName, QScriptValue a_gain);

    void setCurveOffset(QScriptValue a_dataName, QScriptValue a_offset);

    void pullValue(QScriptValue a_dataName);

    void setLogEnable(QScriptValue a_dataName, QScriptValue a_boolString);

private:
    QMap<QString, LDataValue> m_mapPointValues;
};

#endif // LPOINTAPI_H
