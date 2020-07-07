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
    const unsigned sensorFOV_Y = 39; //Deg
    const unsigned sensorFOV_X = 10; //Deg
    unsigned minDegX_now = 0;
    unsigned minDegY_now = 35;
    unsigned maxDegX_now = 110;
    unsigned maxDegY_now = 110; //toDo: keep aspectratio
    QTimer* ComTimer;
    int timeOutTime = 3000;
    bool fastMode = false;

    ThermalImage *tImgP;
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
        ctRESPOK = 5,
        ctRESPER = 6
    };

    explicit thermalComHandler(QObject *parent = nullptr);
    ~thermalComHandler();

    const QStringList& getResolutionList() const;

    void setMinDegX_now(const unsigned &value);

    void setMinDegY_now(const unsigned &value);

    bool fillCommandList(const QString &Res);

    void setTImgP(ThermalImage *value);

    void handleLoopCallback(const QByteArray& msg);

    ThermalImage *getTImgP() const;

    void setFastMode(bool value);

signals:
    void sendSingleCommandS(command comm);
    void startStopLoopSignal(bool state);
    void sendCommandMessage(QByteArray msg);
    void newImageData();
    void newMinMax();
    void connectionError(QString errorMsg);

public slots:
    void handleSendSingleCommand(const command& comm);
    void handleSendLoopStartCommand();
    void handleSingleCallback(const QByteArray& msg);
    void handleCommandCallback(const QByteArray& msg);
    void handleStartStopSignal(bool state, QString res);
    void handleComTimeout();
};

#endif // THERMALCOMHANDLER_H
