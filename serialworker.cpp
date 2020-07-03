#include "serialworker.h"

SerialWorker::SerialWorker(QObject *parent) : QObject(parent)
{
    m_SerialPort = nullptr;
}

SerialWorker::~SerialWorker()
{
    this->ClosePort();
}


void SerialWorker::ConnectToPort(QString sPortName, int iBaudRate, int iDataBits, int iParity, int iStopBits)
{
    m_sPortName = sPortName;
    m_pBaudRate = QSerialPort::BaudRate(iBaudRate);
    m_pDataBits = QSerialPort::DataBits(iDataBits);
    m_pParity = QSerialPort::Parity(iParity);
    m_pStopBits = QSerialPort::StopBits(iStopBits);

    m_SerialPort = new QSerialPort(m_sPortName);
    m_rxTimer = new QTimer();

    if(m_SerialPort)
    {
        if (!m_SerialPort->open(QIODevice::ReadWrite))
        {
            emit ErrorString("Couldn´t open Port");
        }
    }
        if(!m_SerialPort->setBaudRate(m_pBaudRate))
        {
            emit ErrorString("Baudrate Error");
        }
        if(!m_SerialPort->setParity(m_pParity))
            emit ErrorString("Parity Error");
        if(!m_SerialPort->setDataBits(m_pDataBits))
            emit ErrorString("DataBits Error");
        if(!m_SerialPort->setStopBits(m_pStopBits))
            emit ErrorString("StopBits Error");
    if (m_SerialPort->isOpen())
    {
        emit ErrorString("Port Opened");
        connect(m_SerialPort,SIGNAL(readyRead()),this,SLOT(handleReadyRead()));
        connect(m_SerialPort,SIGNAL(errorOccurred(QSerialPort::SerialPortError)),this,SLOT(handleError(QSerialPort::SerialPortError)));
        connect(m_rxTimer, SIGNAL(timeout()), this, SLOT(handleRxTimeout()));
        emit connected();
    }

}

void SerialWorker::sendMessage(QByteArray txMessage)
{
    qDebug()<< "Send Message: " << txMessage;
    if(m_SerialPort != nullptr)
    {
        if(m_SerialPort->isOpen())
        {
            qint64 sendCount = 0;
            int MessageCount = txMessage.length();
            sendCount = m_SerialPort->write(txMessage);

            if (sendCount == MessageCount)
            {
                emit messageSendComplete();
                emit ErrorString("Send Message sucessfull");
            }
            else
            {
                m_SerialPort->flush();
            }
        }
        else emit ErrorString("couldn´t send no Port open!");
    }
    else emit ErrorString("couldn´t send no Port open!");
}

void SerialWorker::handleReadyRead()
{
    QByteArray rxMessage = m_SerialPort->readAll();
    m_rxMessageList.append(rxMessage);
    m_rxTimer->start(20);
}

void SerialWorker::ClosePort()
{
    if(m_SerialPort != nullptr)
    {

        if(m_SerialPort->isOpen())
        {
            m_SerialPort->close();
            if (!m_SerialPort->isOpen())
            {
                emit ErrorString("Port closed");
                emit closed();
                m_SerialPort->deleteLater();
                m_SerialPort = nullptr;
            }
        }
        else
        {
            emit ErrorString("port already closed");
            emit closed();
        }
    }
    else
    {
        emit ErrorString("no port exists");
        emit closed();
    }
}

void SerialWorker::handleError(QSerialPort::SerialPortError error)
{
    QSerialPort::SerialPortError pError = error;
    m_pErrorList.append(pError);
    emit ErrorString(QString("Serial Error happend: %1").arg(m_SerialPort->errorString()));
}

void SerialWorker::handleRxTimeout()
{
    m_rxTimer->stop();
    QByteArray rxMessage = m_rxMessageList.join();
    qDebug()<< "received Message: " <<rxMessage;
    emit messageReceived(rxMessage);
    m_rxMessageList = {};
}
