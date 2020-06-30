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
        unsigned xStep = static_cast<unsigned>(sensorFOV_X); // no interlacing
        unsigned yStep = static_cast<unsigned>(sensorFOV_Y);
        unsigned startDegX = minDegX_now+sensorFOV_X/2;
        unsigned startDegY = minDegY_now+sensorFOV_Y/2;
        for(unsigned y = 0; y < mCountY; y++)
        {
            if(mCountY>1)
            {
                unsigned deg = startDegY+ y*yStep; //invert y axisS
                deg = 300 - deg;
                command tempComm(ctGOTOTILT,deg);
                commandList.append(tempComm);
            }
            for(unsigned x = 0; x< mCountX; x++)
            {
                unsigned deg = 0;
                if(true)
                {
                    deg = startDegX+x*xStep;
                    command tempComm(ctMEASATROT,deg,x,y);
                    commandList.append(tempComm);
                }
                else
                {
                    deg = startDegX+(mCountX -1-x)*xStep;
                    command tempComm(ctMEASATROT,deg,mCountX - 1- x,y);
                    commandList.append(tempComm);
                }

            }
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
        //unsigned midX = (maxDegX_now-minDegX_now)/2;
        //unsigned midY = (maxDegY_now-minDegY_now)/2;
        bool status = fillCommandList(res);
        //goto middle Position
        if(status)
        {
            qDebug() << "OK";
            //command homeX(ctGOTOROT,midX); // add Single Command handling
            //command homeY(ctGOTOTILT,midY);
            //emit sendSingleCommandS(homeX);
            //emit sendSingleCommandS(homeY);
            if(tImgP != nullptr)
            {
                //delete tImgP;
                tImgP = nullptr;
            }
            tImgP = new ThermalImage(resX,resY);
            connect(tImgP, SIGNAL(newMinMax()), this, SIGNAL(newMinMax()));
            handleSendLoopStartCommand();

        }

    }
    else
    {
        delete tImgP;
        loopCount = 0;
        commandList.clear();

    }
    loopRunning = state;
}

void thermalComHandler::handleComTimeout()
{
    loopRunning = false;
    loopCount = 0;
    commandList.clear();
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
        for(int xc = 0; xc < static_cast<int>(sensorResX); xc++ )
        {
            QStringList subStrL = strL[xc+1].split(",");
            QVector<float> tvec;
            for (int yc = 0; yc< static_cast<int>(sensorResY); yc++)
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
    tImgP = new ThermalImage(4,16);
    connect(this, SIGNAL(sendSingleCommandS(command)), this, SLOT(handleSendSingleCommand(command)));
    connect(tImgP, SIGNAL(newMinMax()), this, SIGNAL(newMinMax()));
}

thermalComHandler::~thermalComHandler()
{
    delete  tImgP;
}

void thermalComHandler::handleSendSingleCommand(const command& comm)
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
    if(loopRunning)
    {
        command nowComm = commandList[loopCount];
        QString name = nowComm.getComm().mid(0,2);
        if(name == command::getCommandStr(ctMEASATROT))
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
        if(name == command::getCommandStr(ctMEASURE))
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
        if(!msg.startsWith("ER")) //when answer not Error send new Command else send old Command
        {
            if(loopCount<commandList.length()-1)
            {
                loopCount +=1;
            }
            else
            {
                loopCount = 0;
            }
        }
        else
        {
            qDebug()<< "Error in recv. Message while Loop";
        }
        handleSendLoopStartCommand();
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
