#include "LPointApi.h"

LPointApi::LPointApi(QObject *parent) :
    LApi(parent)
{
}

void LPointApi::setDataList(const QList<QString> &a_rDataList)
{
    QList<QString> listTmpNames = m_mapPointValues.keys();

    QListIterator<QString> it(a_rDataList);
    while (it.hasNext()) {
        QString strDataName = it.next();

        if(!m_mapPointValues.contains(strDataName)) {
            // this is new
            LDataValue value;
            m_mapPointValues.insert(strDataName, value);
        }
        else {
            // already has this data
            // remove the selected ones, those left in this list are about to be deleted
            listTmpNames.removeOne(strDataName);
        }
    }

    // delete the old ones which we do not need any more
    QListIterator<QString> j(listTmpNames);
    while (j.hasNext()) {
        QString strDataName = j.next();
        if (m_mapPointValues.contains(strDataName)) {
            m_mapPointValues.remove(strDataName);
        }
    }
}

void LPointApi::updateDataValue(const QString &a_rDataName, const LDataValue &a_rDataValue)
{
    if (m_mapPointValues.contains(a_rDataName)) {
        m_mapPointValues[a_rDataName] = a_rDataValue;
    }
}

void LPointApi::subscribe(QScriptValue a_dataName)
{
    if (a_dataName.isString()) {
        emit sendSubCmd(true, a_dataName.toString());
    }
    else {
        emit apiCallError("Error: The Function \"subscribe(dataName)\" must take a string as argument.");
    }
}

void LPointApi::unsubscribe(QScriptValue a_dataName)
{
    if (a_dataName.isString()) {
        emit sendSubCmd(false, a_dataName.toString());
    }
    else {
        emit apiCallError("Error: The Function \"subscribe(dataName)\" must take a string as argument.");
    }
}

QScriptValue LPointApi::getValue(QScriptValue a_dataName)
{
    if (a_dataName.isString()) {
        QString strDataName = a_dataName.toString();
        if (m_mapPointValues.contains(strDataName)) {
            if (m_mapPointValues[strDataName].getStatus() == LDataValue::eNotAvailable) {
                return QScriptValue(QScriptValue::NullValue);
            }
            else {
                return QScriptValue(m_mapPointValues[strDataName].getValue());
            }
        }
        else {
            return QScriptValue(QScriptValue::NullValue);
        }
    }
    else {
        emit apiCallError("Error: The Function \"getValue(dataName)\" must take a string as argument.");
        return QScriptValue(QScriptValue::NullValue);
    }
}

void LPointApi::setValue(QScriptValue a_dataName, QScriptValue a_dataValue)
{
    if (a_dataName.isString()) {
        QString strDataName = a_dataName.toString();
        if (m_mapPointValues.contains(strDataName)) {
            if (a_dataValue.isNumber()) {
                // a valid number
                LDataValue dataValue;
                dataValue.set(a_dataValue.toNumber());
                emit sendDataValue(strDataName, dataValue);
            }
            else if (a_dataValue.isString() && a_dataValue.toString() == "NA"){
                // a not available point
                LDataValue dataValue;
                dataValue.setStatus(LDataValue::eNotAvailable);
                emit sendDataValue(strDataName, dataValue);
            }
            else if(a_dataValue.isBool()) {
                // transfer bool data to number
                LDataValue dataValue;
                dataValue.set(a_dataValue.toBool() ? 1 : 0);
                emit sendDataValue(strDataName, dataValue);
            }
            else {
                emit apiCallError("Error: The Function \"setValue(dataName, value)\" must take a string as data name and a number or \"NA\" as data value.");
            }
        }
    }
    else {
        emit apiCallError("Error: The Function \"setValue(dataName, value)\" must take a string as data name and a number or \"NA\" as data value.");
    }
}

void LPointApi::setCurveEnable(QScriptValue a_dataName, QScriptValue a_boolString)
{
    if (a_dataName.isString() && a_boolString.isBool()) {
        QString strDataName = a_dataName.toString();
        bool bIsEnable = a_boolString.toBool();
        emit sendCurveEnable(strDataName, bIsEnable);
    }
    else {
        emit apiCallError("Error: The Function \"setCurveEnable(dataName, isEnable)\" must take a string as data name and a bool as enable/disable.");
    }
}

void LPointApi::setCurveColor(QScriptValue a_dataName, QScriptValue a_colorString)
{
    if (a_dataName.isString() && a_colorString.isString()) {
        QString strDataName = a_dataName.toString();
        QString strColor = a_colorString.toString();
        // transfer to color from string
        QColor color;
        bool ok;
        color.setRed(strColor.mid(1,2).toInt(&ok,16));
        color.setGreen(strColor.mid(3,2).toInt(&ok,16));
        color.setBlue(strColor.mid(5,2).toInt(&ok,16));
        if (ok) {
            emit sendCurveColorCmd(strDataName, color);
        }
    }
    else {
        emit apiCallError("Error: The Function \"setCurveColor(dataName, color)\" must take a string as data name and a RGB color string as color.");
    }
}

void LPointApi::setCurveGain(QScriptValue a_dataName, QScriptValue a_gain)
{
    if (a_dataName.isString() && a_gain.isNumber()) {
        QString strDataName = a_dataName.toString();
        float fGain = a_gain.toNumber();
        emit sendCurveGainCmd(strDataName, fGain);
    }
    else {
        emit apiCallError("Error: The Function \"setCurveGain(dataName, gain)\" must take a string as data name and a number as gain.");
    }
}

void LPointApi::setCurveOffset(QScriptValue a_dataName, QScriptValue a_offset)
{
    if (a_dataName.isString() && a_offset.isNumber()) {
        QString strDataName = a_dataName.toString();
        float fOffset = a_offset.toNumber();
        emit sendCurveOffsetCmd(strDataName, fOffset);
    }
    else {
        emit apiCallError("Error: The Function \"setCurveOffset(dataName, offset)\" must take a string as data name and a number as offset.");
    }
}

void LPointApi::pullValue(QScriptValue a_dataName)
{
    if (a_dataName.isString()) {
        QString strDataName = a_dataName.toString();
        emit sendPullValueCmd(strDataName);
    }
    else {
        emit apiCallError("Error: The Function \"pullValue(dataName)\" must take a string as data name.");
    }
}

void LPointApi::setLogEnable(QScriptValue a_dataName, QScriptValue a_boolString)
{
    if (a_dataName.isString() && a_boolString.isBool()) {
        QString strDataName = a_dataName.toString();
        bool bIsEnable = a_boolString.toBool();
        emit sendLogEnable(strDataName, bIsEnable);
    }
    else {
        emit apiCallError("Error: The Function \"setLogEnable(dataName, isEnable)\" must take a string as data name and a bool as enable/disable.");
    }
}
