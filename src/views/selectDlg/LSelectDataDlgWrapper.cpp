#include "LSelectDataDlgWrapper.h"

LSelectDataDlgWrapper::LSelectDataDlgWrapper()
{
    m_strRegisterName = "SelectedNames";
}

LSelectDataDlgWrapper::~LSelectDataDlgWrapper()
{
}

void LSelectDataDlgWrapper::setDataNameCollection(const QStringList &a_rDataNameCol)
{
    m_pDialog->setDataNameCollection(a_rDataNameCol);
}

void LSelectDataDlgWrapper::createDialog(QWidget* parent)
{
    m_pDialog = new LSelectDataDlg(parent);
    m_pDialog->setAttribute(Qt::WA_DeleteOnClose);
    m_pDialog->setWindowTitle(tr("Select Data"));
    connect(m_pDialog, &LSelectDataDlg::acceptSelection, this, &LSelectDataDlgWrapper::getAcceptSig);
}

void LSelectDataDlgWrapper::execDialog()
{
    m_pDialog->exec();
}

void LSelectDataDlgWrapper::clickOk()
{
    m_pDialog->clickOk();
}

void LSelectDataDlgWrapper::getAcceptSig(QList<QString> a_listNames)
{
    m_listHistoryNames = a_listNames;

    convertToSetting();

    emit triggerAcceptSelection(m_listHistoryNames);

    if (m_pDialog != NULL) {
        delete m_pDialog, m_pDialog = NULL;
    }
}


void LSelectDataDlgWrapper::parseFromSetting(LObixTreeIter a_obixIter)
{
    m_listHistoryNames.clear();

    QList<QString> listNames;
    a_obixIter.moveToRoot();
    a_obixIter.moveToFirstChild();
    if (a_obixIter.getValue().getType() == eObj)  {
        listNames.append(a_obixIter.getValue().getProperty("name"));
    }
    while(a_obixIter.hasSibling()) {
        a_obixIter.moveToNextSibling();
        if (a_obixIter.getValue().getType() == eObj)  {
            listNames.append(a_obixIter.getValue().getProperty("name"));
        }
    }
    m_listHistoryNames = listNames;
}

void LSelectDataDlgWrapper::convertToSetting()
{
    m_obixMutableIter.moveToRoot();
    // build a new subtree
    if (m_obixMutableIter.hasChild())
        m_obixMutableIter.removeChildren();
    QListIterator<QString> i(m_listHistoryNames);
    while (i.hasNext()) {
        QString strDataName = i.next();
        LObixObj obixObj("obj", "name", strDataName);
        m_obixMutableIter.appendChild(obixObj);
    }

}

void LSelectDataDlgWrapper::loadHistory()
{
    //force dialog to apply history settings
    m_pDialog->loadHistory(m_listHistoryNames);
}

void LSelectDataDlgWrapper::selectName(const QString &a_rstrDataName)
{
    m_pDialog->selectName(a_rstrDataName);
}

void LSelectDataDlgWrapper::unselectName(const QString &a_rstrDataName)
{
    m_pDialog->unselectName(a_rstrDataName);
}
