#include "LMainWindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include <QStyleFactory>
#include "LUniqueResource.h"
#include "LSelectProjectDialog.h"
#include "LPath.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Set the style of App
    a.setStyle(QStyleFactory::create("fusion"));

    // Set the current path and fix it to avoid the problem of changing current path on the fly
    PATH_ACCESSER->setCurPath();

    // Show splash
    QSplashScreen splash(QPixmap(PATH_ACCESSER->getPath(ePathImage, "lobster.png")));
    splash.show();
    QCoreApplication::processEvents();

    // Get project file
    LSelectProjectDialog* pSelectProjDiag = new LSelectProjectDialog;
    splash.finish(pSelectProjDiag);
    if(pSelectProjDiag->exec() == QDialog::Accepted) {
        PATH_ACCESSER->setProjectPath(pSelectProjDiag->getProjectPath());
        delete pSelectProjDiag;
    }
    else {
        delete pSelectProjDiag;
        return 0;
    }

    // Show splash for a few seconds
    splash.show();
    QCoreApplication::processEvents();

    // Main window instance
    LMainWindow w;
    w.show();
    splash.finish(&w);

    return a.exec();
}
