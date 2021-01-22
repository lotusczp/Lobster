#ifndef LWATCHWINDOWITEM_H
#define LWATCHWINDOWITEM_H

#include <QObject>
#include <QString>
#include <QColor>

class LWatchWindowItem : public QObject
{
    Q_OBJECT
public:
    LWatchWindowItem(const QString& a_rDataName);

    const QString& getName() const;

    const QString& getValue() const;
    void setValue(const QString& a_strValue);

    bool isScopeEnable() const;
    void setScopeEnable(bool a_bEnable);

    const QColor& getCurveColor() const;
    void setCurveColor(const QColor &a_rColor);

    bool isLogEnable() const;
    void setLogEnable(bool a_bEnable);

    LWatchWindowItem &operator=(const LWatchWindowItem &other);

signals:
    void sendScopeEnableStateByItem(QString name, bool state);

    void sendCurveColorChangedByItem(QString name);

    void sendLogEnableStateByItem(QString name, bool state);

public slots:
    void getScopeCheckState(bool a_bState);
    void getLogCheckState(bool a_bState);

    void getColorChanged(QColor color);

private:
    LWatchWindowItem();

private:
    QString m_strDataName;      //!< name of this data

    QString m_strValue;         //!< value of this data in string format

    bool m_bScopeEnabled;       //!< enable this data shown on scope

    QColor m_curveColor;        //!< color of the curve

    bool m_bLogEnabled;         //!< enable this data been logged

};

#endif // LWATCHWINDOWITEM_H
