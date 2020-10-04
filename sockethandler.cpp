#include "sockethandler.h"
#include <QTime>
#include <QTimer>


socketHandler::socketHandler(QObject *parent) : QObject(parent)
{
    p_sock = nullptr;
    con_state = false;
    tempMsg.clear();
    connect(&recvTimer,SIGNAL(timeout()), this, SLOT(handle_recvTimeout()));
    connect(&connectTimer,SIGNAL(timeout()), this, SLOT(handle_connectTimeout()));
}

bool socketHandler::getCon_state() const
{
    return con_state;
}

void socketHandler::setCon_state(bool value)
{
    con_state = value;
}

socketHandler::~socketHandler()
{
    if(p_sock)
    {
        delete p_sock;
        p_sock = nullptr;
    }
}

void socketHandler::connectToServer(bool state, QString ip, QString port)
{
    if(state == 1)
    {
        if(p_sock == nullptr)
        {
            p_sock = new QTcpSocket(this);
            //connect(p_sock, SIGNAL(connected()), this , SIGNAL(socketConnected()));
            connect(p_sock,SIGNAL(readyRead()), this, SLOT(handle_readyRead()));
        }
        socketIp = ip;
        socketPort = static_cast<quint16>(port.toInt());
        p_sock->connectToHost(ip,static_cast<quint16>(port.toInt()));
        connectTimer.start(5000);
    }
    else
    {
        if(p_sock!= nullptr)
        {
            p_sock->close();
            if(p_sock->state() != QAbstractSocket::ConnectedState)
            {
                disconnect(p_sock,SIGNAL(readyRead()), this, SLOT(handle_readyRead()));
                //disconnect(p_sock, SIGNAL(connected()), this , SIGNAL(socketConnected()));
                emit socketClosed();
                emit errorMsg("Connection closed");
                con_state = false;
                delete p_sock;
                p_sock = nullptr;
            }
        }
    }
}

void socketHandler::handle_sendData(QByteArray data)
{

    if(p_sock->state() == QAbstractSocket::ConnectedState)
    {
        p_sock->write(data);
        p_sock->waitForBytesWritten();
    }
    else
    {
        qDebug()<< p_sock->state() << endl;
    }
}

void socketHandler::handle_readyRead()
{
    QString msg = p_sock->readLine();
    tempMsg.append(msg);
    recvTimer.start(20);

}

void socketHandler::handle_recvTimeout()
{
    recvTimer.stop();
    QString msgComplete = tempMsg.join("");
    if( msgComplete.endsWith("\n"))
    {
        emit data_received(msgComplete.toUtf8());
        tempMsg.clear();
    }

}

void socketHandler::handle_connectTimeout()
{
    connectTimer.stop();
    QAbstractSocket::SocketState state = p_sock->state();
    if(state == QAbstractSocket::ConnectedState)
    {
        emit socketConnected();
        con_state = true;
    }
    else
    {
        emit errorMsg("Couldn´t connect, Timeout");
        p_sock->reset();
    }

}
