#include "LOscilloscope.h"
#include "ui_LOscilloscope.h"

#include <QDateTime>

#include "LMainSettingsMgr.h"


LOscilloscope::LOscilloscope(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LOscilloscope)
{
    ui->setupUi(this);

    // connect signals
    connect(ui->menu, &LScopeMenu::sendShowCursor, ui->scope, &LPlotter::showCursor);
    connect(ui->menu, &LScopeMenu::sendShowGrid, ui->scope, &LPlotter::showGrid);
    connect(ui->scope, &LPlotter::sendCursorInfo, ui->menu, &LScopeMenu::receiveCursorInfo);

    // settings
    LSettingsRelier* pSettingsRelier = ui->menu;
    LMainSettingsMgr::instance()->registerToSettingsMgr(pSettingsRelier);

    m_bIsStarted = false;
}

LOscilloscope::~LOscilloscope()
{
    delete ui;
}

void LOscilloscope::updateValue(const QString& a_rDataName, const LDataValue& a_rDataValue)
{
    if(m_bIsStarted) {
#if 0
        if(m_iScopeStartTime == -1) {
            m_iScopeStartTime = a_rDataValue.getAbsTime();
        }
        m_dCurrentXTime = (double(a_rDataValue.getAbsTime() - m_iScopeStartTime))/1000.0;

        // if x-axis is full, restart at the beginning
        if (m_dCurrentXTime >= ui->scope->getMaxX()) {
            // this indicate a whole new original point
            m_dCurrentXTime = 0;
            m_iScopeStartTime = a_rDataValue.getAbsTime();
            ui->scope->clearAllCurvePoints();
        }
#else
        if(m_iScopeStartTime == -1) {
            m_iScopeStartTime = a_rDataValue.getAbsTime();
        }

        double dElapsedTime = (double(a_rDataValue.getAbsTime() - m_iScopeStartTime))/1000.0;
        if (dElapsedTime >= ui->scope->getMaxX()) {
            ui->scope->setMaxX(dElapsedTime);
        }

#endif
        // We can only display valid values
        if (a_rDataValue.getStatus() == LDataValue::eAvailable) {
            // insert the current value
            ui->scope->addCurvePoint(a_rDataName, QPointF(dElapsedTime , a_rDataValue.getValue()));
        }
    }
}

void LOscilloscope::start()
{
    // clear all first
    ui->scope->clearAllCurvePoints();

    m_bIsStarted = true;
#if 1
    m_iScopeStartTime = -1;
#else
    m_iScopeStartTime = QDateTime::currentMSecsSinceEpoch();
#endif
    ui->scope->startRefresh(true);
}

void LOscilloscope::stop()
{
    m_bIsStarted = false;
    ui->scope->startRefresh(false);
}

void LOscilloscope::addNewCurve(const QString &a_rDataName)
{
    ui->scope->addNewCurve(a_rDataName);
    // set the curve to be solid
    ui->scope->setCurveStyle(a_rDataName, Qt::SolidLine);

}

void LOscilloscope::eraseCurve(const QString& a_rDataName)
{
    ui->scope->deleteCurve(a_rDataName);
}

void LOscilloscope::setCurveColor(const QString &a_rName, const QColor &a_rColor)
{
    ui->scope->setCurveColor(a_rName, a_rColor);
}

void LOscilloscope::setCurveSelected(const QString &a_rName, bool a_bSelected)
{
    ui->scope->setCurveSelected(a_rName, a_bSelected);
}

void LOscilloscope::mergeValueAxis(QList<QString> a_listMergedName)
{
    ui->scope->mergeValueAxis(a_listMergedName);
}

void LOscilloscope::demergeValueAxis(QList<QString> a_listName)
{
    ui->scope->demergeValueAxis(a_listName);
}

void LOscilloscope::setCurveYGain(const QString &a_rName, float a_fCurveYGain)
{
    ui->scope->setCurveYGain(a_rName, a_fCurveYGain);
}

void LOscilloscope::setCurveYOffset(const QString &a_rName, float a_fCurveYOffset)
{
    ui->scope->setCurveYOffset(a_rName, a_fCurveYOffset);
}
