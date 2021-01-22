#ifndef LDATAMANAGER_H
#define LDATAMANAGER_H

// Qt headers
#include <QObject>
#include <QMap>
#include <QStringList>

// lobster headers
#include "LData.h"
#include "LCommon.h"




class LDataManager : public QObject
{
    Q_OBJECT
public:
    static LDataManager* instance();

    static void destroy();


    void startActiveChannels();

    void pauseActiveChannels();

    void stopActiveChannels();



    void fetchAllAvailableDataNames();

    const QStringList& getDataNameCollection() const;


    void subscribeDataList(const QMap<QString, QList<QString> > &a_rDataList);

    void unsubscribeChannel(const QString &a_rChannelName);


    void attachValueObserver(const QString& a_rDataName, LValueObserver* a_pObserver);

    void detachValueObserver(const QString& a_rDataName, LValueObserver* a_pObserver);


    void attachAttriObserver(const QString& a_rDataName, LAttributesObserver* a_pObserver);

    void detachAttriObserver(const QString& a_rDataName, LAttributesObserver* a_pObserver);


private:
    LDataManager();
    ~LDataManager();


    void subscribeDataByChannel(const QString &a_rChannelName, const QList<QString>& a_rNamesWithPrefix);

private:
    static LDataManager     *s_pInstance;


    QStringList             m_listDataNameCollections;  //!< all the name read from data source config files

    QMap<QString, LData*>   m_mapDataList;              //!< [data name][data], this is a table of all the data selected by user

    QList<QString>          m_listExistingNames;        //!< a temporary memory of existing data names

    bool                    m_bIsRunning;               //!< the flag for running

signals:
    void sendDataValue(QString a_strDataName, LDataValue a_dDataValue);

    void pullDataValue(QString a_strDataName);

public slots:
    //! Get notify signal for passing received data to user by data list
    void getNotifySingleVar(QString a_strDataName, LDataValueList a_lstDataValue);

    //! Get notify signal for passing received data to user by a map of data
    void getNotifyMultiVar(LDataValueMap a_mapDataValue);

    //! Receive user-write value
    void receiveDataValue(QString a_strDataName, LDataValue a_dDataValue);

    //! Receive user-pull value
    void receivePullValueCmd(QString a_strDataName);

    //! Get notify signal for variable attributes
    void getNotifyVarAttributes(LAttributesMap a_mapAttributes);

};

#endif // LDATAMANAGER_H
