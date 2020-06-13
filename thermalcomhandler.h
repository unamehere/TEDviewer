#ifndef THERMALCOMHANDLER_H
#define THERMALCOMHANDLER_H

#include <QObject>
#include "backroundworker.h"
#include "command.h"
#include "thermalimage.h"

#define COM_BAUDRATE QSerialPort::Baud115200

class thermalComHandler : public QObject
{
    Q_OBJECT
    const QStringList resolution = {"4x16", "16x16", "32x32", "64x64","64x48"};
    const unsigned sensorResY = 16; //Pxls
    const unsigned sensorResX = 4; //Pxls
    const unsigned sensorFOV_Y = 40; //Deg
    const unsigned sensorFOV_X = 10; //Deg
    const unsigned minDegX = 0; //constants of the min and max Degrees of the Hardware
    const unsigned minDegY = 0;
    const unsigned maxDegX = 300;
    const unsigned maxDegY = 300;
    unsigned minDegX_now = 20;
    unsigned minDegY_now = 20;
    unsigned maxDegX_now = 110;
    unsigned maxDegY_now = 110; //toDo: keep aspectratio

    ThermalImage *tImgP;

    bool msgType = mtSingle;
    bool singleCallbackReceived = false;

    //Looping stuff
    bool loopRunning = false;
    unsigned loopCount = 0;
    QList<command> commandList;

    bool translateBAtoFLoatVect(const QByteArray& ba,QVector<QVector<float>>* vals);
    QByteArray translateCommandtoBA(const command& comm);
public:
    enum CommandType
    {
        ctGOTOROT = 0,
        ctGOTOTILT = 1,
        ctMEASATROT = 2,
        ctMEASATTILT = 3,
        ctMEASURE = 4,
    };
    enum MsgType
    {
        mtSingle = 0,
        mtLoop = 1
    };

    explicit thermalComHandler(QObject *parent = nullptr);
    ~thermalComHandler();

    const QStringList& getResolutionList() const;

    void setMinDegX_now(const unsigned &value);

    void setMinDegY_now(const unsigned &value);

    void setMaxDegX_now(const unsigned &value);

    void setMaxDegY_now(const unsigned &value);

    bool fillCommandList(const QString &Res);

    void setTImgP(ThermalImage *value);

    void handleLoopCallback(const QByteArray& msg);

    ThermalImage *getTImgP() const;

signals:
    void sendSingleCommandS(command comm);
    void startStopLoopSignal(bool state);
    void sendCommandMessage(QByteArray msg);
    void newImageData();

public slots:
    void handleSendSingleCommand(const command& comm);
    void handleSendLoopStartCommand();
    void handleSingleCallback(QByteArray msg);
    void handleCommandCallback(QByteArray callbackStr);
    void handleStartStopSignal(bool state, QString res);
};

#endif // THERMALCOMHANDLER_H
