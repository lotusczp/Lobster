#ifndef LCHANNELMGRDLGWRAPPER_H
#define LCHANNELMGRDLGWRAPPER_H

#include "LChannelMgrDlg.h"
#include "LSettingsRelier.h"

typedef QList<QString> LDeletedChannelList;


class LChannelMgrDlgWrapper : public QObject, public LSettingsRelier
{
    Q_OBJECT
public:
    LChannelMgrDlgWrapper();
    ~LChannelMgrDlgWrapper();

    void loadHistory();


    void createDialog(QWidget* parent);

    void execDialog();


    //! an external command to click ok
    void clickOk();

signals:
    void sendDeletedChannels(LDeletedChannelList a_listChannelNames);

public slots:
    void getAcceptSig(QMap<QString, ChannelContent> a_mapChannelContent);

protected:
    virtual void parseFromSetting(LObixTreeIter a_obixIter) Q_DECL_OVERRIDE;

    virtual void convertToSetting() Q_DECL_OVERRIDE;

private:
    LChannelMgrDlg* m_pDialog;

    QMap<QString, ChannelContent> m_mapChannelContent;

};

#endif // LCHANNELMGRDLGWRAPPER_H
