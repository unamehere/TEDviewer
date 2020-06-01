#ifndef BACKROUNDWORKER_H
#define BACKROUNDWORKER_H

#include <QObject>
#include "serialworker.h"
#include <QSerialPortInfo>
#include <QDateTime>

/*this class initialize the gui with data
 * and makes the communication with the other classes
 */

#define READCALL "R_READ_ALL\n"
#define WRITESTART "WS"
#define WRITEEND "WE"
#define READSTART "RS"
#define READEND "RE"
#define SPLITPARA "#"
#define SPLITVALUE ";"


class backroundworker : public QObject
{
    Q_OBJECT
private:
    SerialWorker *m_worker;
    QThread *WorkerThread;
    bool connected;
public:
    explicit backroundworker(QObject *parent = nullptr);
    ~backroundworker();
    void Startup();
    QList<QString> GetComPorts();

    bool getConnected() const;
    void setConnected(bool value);

signals:
    void connectPort(QString iPortname, int Baudrate, int iDatabits, int iParity, int iStopBits);
    void ClosePort();
    void ErrorMessageFF(QString Message);
    void sendMessageCom(QByteArray TXMessage);
    void ConnectedPort();
    void closedPort();
    void readSuccess();

public slots:
    void newRxMessageCom(QByteArray RxMessage);
    void comRead();
};

#endif // BACKROUNDWORKER_H
