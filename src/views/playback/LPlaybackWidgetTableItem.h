#ifndef LPLAYBACKWIDGETTABLEITEM_H
#define LPLAYBACKWIDGETTABLEITEM_H

#include <QObject>
#include <QString>
#include <QColor>

class LPlaybackWidgetTableItem : public QObject
{
    Q_OBJECT

public:
    LPlaybackWidgetTableItem(const QString& a_rDataName);

    const QString& getName() const {return m_strDataName;}

    const QColor& getCurveColor() const {return m_curveColor;}
    void setCurveColor(const QColor &a_rColor) {m_curveColor = a_rColor;}

    LPlaybackWidgetTableItem &operator=(const LPlaybackWidgetTableItem &other);

signals:
    void sendCurveColorChangedByItem(QString name);

public slots:
    void getColorChanged(QColor color);

private:
    LPlaybackWidgetTableItem();

private:
    QString m_strDataName;      //!< name of this data

    QColor m_curveColor;        //!< color of the curve
};

#endif // LPLAYBACKWIDGETTABLEITEM_H
