#ifndef LWATCHWINDOW_H
#define LWATCHWINDOW_H

#include <QDockWidget>
#include <QTimer>
#include <QMenu>

#include "LWatchWindowItem.h"
#include "LValueObserver.h"
#include "LSettingsRelier.h"
#include "LAttributesObserver.h"

namespace Ui {
class LWatchWindow;
}

class LWatchWindow : public QDockWidget, public LValueObserver, public LSettingsRelier, public LAttributesObserver
{
    Q_OBJECT

public:
    explicit LWatchWindow(QWidget *parent = 0);
    ~LWatchWindow();

    //! inherit from LValueObserver
    virtual void updateValue(const QString& a_rDataName, const LDataValue& a_rDataValue) Q_DECL_OVERRIDE;

    //! inherit from LValueObserver
    virtual void start() Q_DECL_OVERRIDE;

    //! inherit from LValueObserver
    virtual void stop() Q_DECL_OVERRIDE;


    //! inherit from LAttributeObserver
    virtual void updateAttributes(const QString &a_rDataName, const LAttributes &a_rAttributes) Q_DECL_OVERRIDE;


    //! inherit from LSettingsRelier
    virtual void parseFromSetting(LObixTreeIter a_obixIter) Q_DECL_OVERRIDE;

    //! inherit from LSettingsRelier
    virtual void convertToSetting() Q_DECL_OVERRIDE;


    //! set data list to the watch window
    void setDataList(const QList<QString> &a_rNameList);

signals:
    void sendScopeEnableState(QString name, bool state);
    void sendCurveColor(QString name, QColor curveColor);
    void sendCurveSelected(QString name, bool selected);
    void sendLogEnableState(QString name, bool state);
    void sendDataValue(QString a_strDataName, LDataValue a_dDataValue);
    void sendPullValueCmd(QString a_strDataName);
    void sendMergeValueAxis(QList<QString> names);
    void sendDemergeValueAxis(QList<QString> names);

public slots:
    // these are slots for inside items
    void receiveScopeEnableStateByItem(QString name, bool state);
    void receiveCurveColorChangedByItem(QString name);
    void receiveLogEnableStateByItem(QString name, bool state);

    // these are slots for outside
    void receiveScopeEnableState(QString name, bool state);
    void receiveCurveColor(QString name, QColor curveColor);
    void receiveLogEnableState(QString name, bool state);

private:
    void addItemLine(const LWatchWindowItem& a_rItem);

    void deleteItemLine(const QString &a_rItemName);

    void clearHistoryItems();

    void updateEnumsBox(const QString& a_rstrName, int a_iValue);

private slots:
    void refreshValue();

    void receiveEnumChanged(int index);

    void handleCustomContextMenu(const QPoint &pos);

    void handleMergeValueAxis();

    void handleDemergeValueAxis();

    void on_tableWidget_cellChanged(int row, int column);

    void on_tableWidget_cellClicked(int row, int column);

    void on_tableWidget_cellDoubleClicked(int row, int column);

private:
    class Timeout {
    public:
        Timeout(unsigned int a_uMaxTime) {m_uMaxTime = a_uMaxTime; m_uCount = 0; flag = true;}
        ~Timeout() {}
        void reset() {m_uCount = 0; flag = false;}
        void tick()
        {
            if (!flag) {
                m_uCount++;
                if (m_uCount == m_uMaxTime)
                    flag = true;
            }
        }
        bool isTimeout() const {return flag;}
    private:
        Timeout() {}
        unsigned int m_uCount;
        unsigned int m_uMaxTime;
        bool flag;
    };

private:
    Ui::LWatchWindow *ui;

    unsigned int m_uRowCount;

    QList<QString> m_listTmpNames;        //!< a temporary memory of existing data names

    QMap<QString, LWatchWindowItem*> m_mapItems;    //!< a map of all items shown in the watch window

    QMap<QString, LWatchWindowItem*> m_mapHistoryItems; //!< a temporary memory of history items information, in case the current selection is different


    QTimer *m_pRefreshTimer;    //!< timer used to refresh real-time values


    QMap<QString, Timeout*> m_pTimeoutMap;  //!< a map of timer to indicate each value's timeout occasion, which is related to the background color

    QString m_strSelectedName;  //!< the name of the data which has been selected by mouse click

    QMenu   *m_pNameContextMenu;
};

#endif // LWATCHWINDOW_H
