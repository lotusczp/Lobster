#ifndef LCHANNELMGRDLG_H
#define LCHANNELMGRDLG_H

#include <QDialog>
#include <QMap>
#include <QListWidgetItem>

namespace Ui {
class LChannelMgrDlg;
}


struct ChannelContent {
    QString name;
    QString pluginPath;
    QString configPath;
};

typedef QMap<QString, ChannelContent>           ChannelContentMap;
typedef QMapIterator<QString, ChannelContent>   ChlMapIter;

class LChannelMgrDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LChannelMgrDlg(QWidget *parent = 0);
    ~LChannelMgrDlg();

    void loadHistory(const QMap<QString, ChannelContent>& a_rMapChannelContent);

    void clickOk();

signals:
    void acceptChannel(QMap<QString, ChannelContent> a_mapChannelContent);

public slots:
    void getNewChannel(QString a_strName, QString a_strConfigPath, QString a_strPluginPath);

private slots:
    void on_addChannel_clicked();

    void on_deleteChannel_clicked();

    void on_buttonBox_accepted();

    void on_listChannelNames_itemClicked(QListWidgetItem *item);

    void on_listChannelNames_itemSelectionChanged();

    void on_lineEditName_editingFinished();

    void on_lineEditConfigPath_editingFinished();

    void on_lineEditPluginPath_editingFinished();

    void on_reloadChannel_clicked();

    void on_reloadAllChannels_clicked();

private:
    void refreshChannelList();

    //! if apply channel contents succeed, return true, else return return false
    bool applyChannelContent(const ChannelContentMap& a_rmapChannelContent);

    //! delete this channel content by user
    void deleteChannel(const QString& a_rstrChannelName);

private:
    Ui::LChannelMgrDlg *ui;

    ChannelContentMap m_mapChannelContent;  //!< a map of transmission content

};

#endif // LCHANNELMGRDLG_H
