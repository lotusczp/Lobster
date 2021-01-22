#include "LErrorHandler.h"

#include <QMessageBox>

LErrorHandler* LErrorHandler::s_pInstance = NULL;

LErrorHandler *LErrorHandler::instance()
{
    // Do we have an oracle yet?
    if(s_pInstance == NULL) {
      // No, create it
      s_pInstance = new LErrorHandler();
    }

    // Return the instance ptr
    return s_pInstance;
}

void LErrorHandler::destroy()
{
    // Clean up the pointer
    if(s_pInstance != NULL) {
        delete s_pInstance, s_pInstance = NULL;
    }
}

void LErrorHandler::handlePluginError(qint32 /*a_iErrorCode*/, QString a_strDescription, QString a_strSource)
{
    QMessageBox::warning(0, QObject::tr("Warning"), "Error From " + a_strSource + ":\n\n" + a_strDescription);
}

LErrorHandler::LErrorHandler()
{
}
