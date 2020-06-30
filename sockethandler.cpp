#include "sockethandler.h"

bool socketHandler::getCon_state() const
{
    return con_state;
}

void socketHandler::setCon_state(bool value)
{
    con_state = value;
}

socketHandler::socketHandler(QObject *parent) : QObject(parent)
{
    p_sock = nullptr;
    con_state = false;
}

socketHandler::~socketHandler()
{
    connectToServer(false);
}

void socketHandler::connectToServer(bool state, QString ip, QString port)
{
    if(state == 1)
    {
        if(p_sock == nullptr)
            p_sock = new QTcpSocket(this);
        connect(p_sock,SIGNAL(readyRead()), this, SLOT(handle_readyRead()));
        socketIp = ip;
        socketPort = static_cast<quint16>(port.toInt());
        p_sock->connectToHost(ip,static_cast<quint16>(port.toInt()));
        while(p_sock->state() == QAbstractSocket::ConnectingState);
        if(p_sock->state() == QAbstractSocket ::ConnectedState)
        {
            con_state = true;
            emit socketConnected();
            emit errorMsg("Connected");
        }
        else
            emit errorMsg("Connection failed");
    }
    else
    {
        p_sock->close();
        if(p_sock->state() != QAbstractSocket::ConnectedState)
        {
            emit socketClosed();
            emit errorMsg("Connection closed");
        }
        if(p_sock != nullptr)
        {
            delete p_sock;
            p_sock = nullptr;
        }
        con_state = false;
    }
}

void socketHandler::handle_sendData(QByteArray data)
{

    if(p_sock->state() == QAbstractSocket::ConnectedState)
    {
        p_sock->write(data);
        p_sock->flush();
    }
    else
    {
        qDebug()<< p_sock->state() << endl;
    }
}

void socketHandler::handle_readyRead()
{
    QByteArray msg = p_sock->readAll();
    emit data_received(msg);
}
