#include "backroundworker.h"
#include "thermalcomhandler.h"


bool backroundworker::getConnected() const
{
    return connected;
}

void backroundworker::setConnected(bool value)
{
    connected = value;
}

backroundworker::backroundworker(QObject *parent) : QObject(parent)
{
    //manual Signal Slot connections
    WorkerThread = new QThread();

    m_worker = new SerialWorker();

    m_worker->moveToThread(WorkerThread);

    //Signal Slots for Comport 1
    connect(m_worker, SIGNAL(ErrorString(QString)),this, SIGNAL(ErrorMessageFF(QString)));
    connect(this,SIGNAL(connectPort(QString, int, int, int, int)),m_worker,SLOT(ConnectToPort(QString, int, int, int, int)));
    connect(m_worker, SIGNAL(connected()),this,SIGNAL(connectedPort()));
    connect(m_worker, SIGNAL(messageReceived(QByteArray)),this, SLOT(newRxMessageCom(QByteArray)));
    connect(m_worker, SIGNAL(messageReceived(QByteArray)),this, SIGNAL(newRXMessageComS(QByteArray)));
    connect(this,SIGNAL(sendMessageCom(QByteArray)),m_worker,SLOT(sendMessage(QByteArray)));
    connect(this,SIGNAL(ClosePort()),m_worker,SLOT(ClosePort()));
    connect(m_worker,SIGNAL(closed()),this,SIGNAL(closedPort()));

    WorkerThread->start();

}

backroundworker::~backroundworker()
{
    emit ClosePort();
    WorkerThread->quit();
    WorkerThread->wait();
}

QList<QString> backroundworker::GetComPorts()
{
    QList <QSerialPortInfo> Portslist = QSerialPortInfo::availablePorts();
    QList <QString> PortsStringList;

    foreach(QSerialPortInfo Port,Portslist)
    {
        PortsStringList.append(Port.portName());
    }
    return PortsStringList;
}



void backroundworker::newRxMessageCom(QByteArray rxMessage)
{
    bool ok = false;
    QString Message = QString(rxMessage);


            if(ok)
                emit readSuccess();

}

void backroundworker::comRead()
{
    //Perform Read and Store Parameters in paraList
    if(this->connected)
    {
        QString call = "";
        emit sendMessageCom(call.toUtf8());
    }
}



