#include "thermalcomhandler.h"

const QStringList& thermalComHandler::getResolution() const
{
    return resolution;
}

thermalComHandler::thermalComHandler(QObject *parent) : QObject(parent)
{
    
}
