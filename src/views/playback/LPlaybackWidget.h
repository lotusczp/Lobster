#ifndef LPLAYBACKWIDGET_H
#define LPLAYBACKWIDGET_H

#include <QWidget>
#include <QMap>
#include <QThread>
#include <QMenu>

#include "LPlaybackWidgetTableItem.h"
#include "LLogFileParser.h"

namespace Ui {
class LPlaybackWidget;
}

class LPlaybackWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LPlaybackWidget(QWidget *parent = nullptr);
    ~LPlaybackWidget();

signals:
    void sendFileToParse(QString filePath);

private slots:
    void receiveCurveColorChangedByItem(QString name);

    void receiveDataValue(QString name, LDataValue value);

    void receiveErrors(QString errorMessage);

    void receiveNames(QStringList valueNames);

    void receiveProcessFinished();

    void receiveProcessPercent(int percent);

    void handleCustomContextMenu(const QPoint &pos);

    void handleMergeValueAxis();

    void handleDemergeValueAxis();

    void on_browseButton_clicked();

    void on_loadButton_clicked();

    void on_tableWidget_cellClicked(int row, int column);


private:
    Ui::LPlaybackWidget *ui;

    unsigned int m_uRowCount;

    qint64 m_iScopeStartTime;

    qreal m_dMaxX;
    qreal m_dMinY;
    qreal m_dMaxY;
    QString m_strSelectedName;

    QMap<QString, LPlaybackWidgetTableItem*> m_mapItems;    //!< a map of all items shown

    QMenu   *m_pNameContextMenu;

    QThread *m_pParserThread;
    LLogFileParser m_logFileParser;
};

#endif // LPLAYBACKWIDGET_H
