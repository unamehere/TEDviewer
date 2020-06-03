#ifndef THERMALCOMHANDLER_H
#define THERMALCOMHANDLER_H

#include <QObject>
#include "backroundworker.h"

#define READCALL "R_READ_ALL\n"
#define WRITESTART "WS"
#define WRITEEND "WE"
#define READSTART "RS"
#define READEND "RE"
#define SPLITPARA "#"
#define SPLITVALUE ";"


class thermalComHandler : public QObject
{
    Q_OBJECT
    const QStringList resolution = {"16x4", "32x32", "64x64","64x48"};
    const unsigned sensorResY = 16; //Pxls
    const unsigned sensorResX = 4; //Pxls
    const unsigned sonsorFOV_Y = 40; //Deg
    const unsigned sensorFOV_X = 10; //Deg
public:
    explicit thermalComHandler(QObject *parent = nullptr);
    const QStringList& getResolution() const;

signals:

public slots:
};

#endif // THERMALCOMHANDLER_H
