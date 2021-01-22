#ifndef LDATA_H
#define LDATA_H

#include <QString>

#include "LDataBuffer.h"
#include "LValueObserver.h"
#include "LAttributesObserver.h"


class LData
{   
public:
    LData(const QString& a_rDataName);
    ~LData();

    //! Get the name of this data
    QString getDataName() const;


    //! Notify all observers with all the values this data currently buffered
    void notifyAllValueObs();

    //! Attach a value observer
    void attachValueObserver(LValueObserver* a_pObserver);

    //! Detach a value observer
    void detachValueObserver(LValueObserver* a_pObserver);


    //! Add the value to buffer, this will change the buffer
    bool pushValue(const LDataValue &a_rValue);

    //! Pop a value from buffer, this will change the buffer
    bool popValue(LDataValue &a_rValue);


    //! Notify all observers with attributes of this data
    void notifyAllAttriObs();

    //! Attach a attributes observer
    void attachAttriObserver(LAttributesObserver* a_pObserver);

    //! Detach a attributes observer
    void detachAttriObserver(LAttributesObserver* a_pObserver);


    //! set the attributes of this data
    void setAttributes(const LAttributes& a_rAttributes);

    //! get the attributes of this data
    const LAttributes& getAttributes() const;


private:
    LData() {}

private:
    QString                     m_strName;          //!< name of data

    LDataBuffer                 m_inputBuffer;      //!< buffer of this data

    LAttributes                 m_attributes;       //!< attributes of this data

    QList<LValueObserver*>      m_listValueObv;     //!< the target apps who need data value

    QList<LAttributesObserver*> m_listAttriObv;     //!< the target apps who need data attributes
};

#endif // LDATA_H
