#include "thermalcomhandler.h"

const QStringList& thermalComHandler::getResolutionList() const
{
    return resolution;
}

void thermalComHandler::setMinDegX_now(const unsigned &value)
{
    minDegX_now = value;
}

void thermalComHandler::setMinDegY_now(const unsigned &value)
{
    minDegY_now = value;
}

void thermalComHandler::setMaxDegX_now(const unsigned &value)
{
    maxDegX_now = value;
}

void thermalComHandler::setMaxDegY_now(const unsigned &value)
{
    maxDegY_now = value;
}

bool thermalComHandler::fillCommandList(const QString &Res)
{
    bool bRetVal = true;
    commandList = {}; // empty CommandList;
    unsigned resX = static_cast<unsigned>(Res.split("x")[0].toInt());
    unsigned resY = static_cast<unsigned>(Res.split("x")[1].toInt());
    unsigned mCountX = resX/this->sensorResX;
    unsigned mCountY = resY/this->sensorResY;
    if(mCountX == mCountY && mCountX == 1)
    {
        command meas(ctMEASURE,0);
        commandList.append(meas);
    }
    else
    {
        unsigned xStep = static_cast<unsigned>((maxDegX_now-minDegX_now-sensorFOV_X)*1.0/mCountX);
        unsigned yStep = static_cast<unsigned>((maxDegY_now-minDegY_now-sensorFOV_Y)*1.0/mCountY);
        unsigned startDegX = minDegX_now+sensorFOV_X/2;
        unsigned startDegY = minDegY_now+sensorFOV_Y/2;
        for(unsigned y = 0; y < mCountY; y++)
        {
            for(unsigned x = 0; x< mCountX; x++)
            {
                unsigned deg;
                if(y%2 == 0)
                {
                    deg = startDegX+x*xStep;
                }
                else
                {
                    deg = startDegX+(mCountX-x)*xStep;
                }
                command tempComm(ctMEASATROT,deg,x,y);
                commandList.append(tempComm);
            }
            unsigned deg = startDegY+y*yStep;
            command tempComm(ctGOTOTILT,deg);
            commandList.append(tempComm);
        }
    }
    return bRetVal;
}

void thermalComHandler::handleStartStopSignal(bool state, QString res)
{
    if(state == true)
    {
        unsigned resX = static_cast<unsigned>(res.split("x")[0].toInt());
        unsigned resY = static_cast<unsigned>(res.split("x")[1].toInt());
        unsigned midX = (maxDegX_now-minDegX_now)/2;
        unsigned midY = (maxDegY_now-minDegY_now)/2;
        bool status = fillCommandList(res);
        //goto middle Position
        if(status)
        {
            qDebug() << "OK";
            command homeX(ctGOTOROT,midX);
            command homeY(ctGOTOTILT,midY);
            //emit sendSingleCommandS(homeX);
            //emit sendSingleCommandS(homeY);
            tImgP = new ThermalImage(resX,resY);
            handleSendLoopStartCommand();

        }

    }
    loopRunning = state;
}

void thermalComHandler::setTImgP(ThermalImage *value)
{
    tImgP = value;
}

ThermalImage *thermalComHandler::getTImgP() const
{
    return tImgP;
}

bool thermalComHandler::translateBAtoFLoatVect(const QByteArray& ba, QVector<QVector<float> > *vals)
{
    vals->clear();
    bool bRetVal = true;
    QStringList strL = QString(ba).split(";");
    if(strL[0] != "TS" || strL.last() != "TE\r\n")
        bRetVal = false;
    if(bRetVal)
    {
        for(int xc = 0; xc < sensorResX; xc++ )
        {
            QStringList subStrL = strL[xc+1].split(",");
            QVector<float> tvec;
            for (int yc = 0; yc< sensorResY; yc++)
            {
                tvec.append(subStrL[yc].toFloat());
            }
            vals->append(tvec);
        }
    }

    return bRetVal;
}

QByteArray thermalComHandler::translateCommandtoBA(const command &comm)
{
    QByteArray ba;
    ba.append(comm.getComm());
    ba.append(QString::number(comm.getDeg()));
    ba.append("\n");
    return ba;
}

thermalComHandler::thermalComHandler(QObject *parent) : QObject(parent)
{
    tImgP = new ThermalImage(16,4);
    connect(this, SIGNAL(sendSingleCommandS(command)), this, SLOT(handleSendSingleCommand(command)));
}

void thermalComHandler::handleSendSingleCommand(command comm)
{
    msgType = mtSingle;
    singleCallbackReceived = false;
    QByteArray msg = translateCommandtoBA(comm);
    emit sendCommandMessage(msg);
}

void thermalComHandler::handleSendLoopStartCommand()
{
    msgType = mtLoop;
    QByteArray msg = translateCommandtoBA(commandList[loopCount]);
    emit sendCommandMessage(msg);
}

void thermalComHandler::handleSingleCallback(QByteArray msg)
{
    singleCallbackReceived = true;
    qDebug()<<msg;
}

void thermalComHandler::handleLoopCallback(const QByteArray& msg)
{
    command nowComm = commandList[loopCount];
    QString name = nowComm.getComm().mid(0,2);
    if(name == "MR" && loopRunning == true)
    {
        if(msg.length()>4)
        {
            QVector<QVector<float>> vect;
            if(translateBAtoFLoatVect(msg,&vect))
            {
                tImgP->addBlock(nowComm.getXPos()*4,nowComm.getYPos()*16,sensorResX,sensorResY,vect);
                emit newImageData();
            }
        }
    }
    if(name == "MM" && loopRunning == true)
    {
        if(msg.length()>4)
        {
            QVector<QVector<float>> vect;
            if(translateBAtoFLoatVect(msg,&vect))
            {
                tImgP->addBlock(nowComm.getXPos(),nowComm.getYPos(),sensorResX,sensorResY,vect);
                emit newImageData();
            }
        }
    }
    if(loopRunning == true)
    {
        if(loopCount<commandList.length()-1)
        {
            loopCount +=1;
        }
        else
        {
            loopCount = 0;
        }
        if(loopRunning)
        {
            handleSendLoopStartCommand();
        }
    }
}

void thermalComHandler::handleCommandCallback(QByteArray msg)
{
    if(msgType == mtSingle)
    {
        handleSingleCallback(msg);
    }
    else
    {
        handleLoopCallback(msg);
    }

}
