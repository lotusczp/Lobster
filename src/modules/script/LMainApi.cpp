#include "LMainApi.h"

LMainApi::LMainApi(QObject *parent) :
    LApi(parent)
{
}

void LMainApi::startTrans()
{
    emit sendCmd(true);
}

void LMainApi::stopTrans()
{
    emit sendCmd(false);
}
