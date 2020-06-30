#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H
#include <QObject>
#include <QTcpSocket>

class socketHandler : public QObject
{
    Q_OBJECT
    QTcpSocket* p_sock;
    bool con_state;
    QString socketIp;
    quint16 socketPort;
public:
    explicit socketHandler(QObject *parent = nullptr);
    ~socketHandler();

    bool getCon_state() const;
    void setCon_state(bool value);

signals:
    void data_received(QByteArray data);
    void socketConnected();
    void socketClosed();
    void errorMsg(QString msg);

public slots:
    void connectToServer(bool state,QString ip = "", QString port = "");
    void handle_sendData(QByteArray data);
    void handle_readyRead();
};

#endif // SOCKETHANDLER_H
