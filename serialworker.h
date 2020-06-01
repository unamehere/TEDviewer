#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QSerialPort>
#include <QThread>
#include <QDebug>

#include <QObject>
#include <QTimer>

class SerialWorker : public QObject
{
    Q_OBJECT
private:
    QSerialPort *m_SerialPort;
    QSerialPort::BaudRate m_pBaudRate;
    QSerialPort::Parity m_pParity;
    QSerialPort::DataBits m_pDataBits;
    QSerialPort::StopBits m_pStopBits;
    QList <QSerialPort::SerialPortError> m_pErrorList;
    QString m_sPortName;
    QList <QByteArray> m_rxMessageList;
    QTimer* m_rxTimer;
public:
    explicit SerialWorker(QObject *parent = nullptr);
    ~SerialWorker();

signals:
    void messageReceived(QByteArray rxMessage);
    void messageSendComplete();
    void connected();
    void closed();
    void ErrorString(QString Message);

public slots:
    void ConnectToPort(QString sPortName, int iBaudRate = QSerialPort::Baud9600, int iDataBits = QSerialPort::Data8, int iParity = QSerialPort::NoParity, int iStopBits = QSerialPort::OneStop);
    void sendMessage(QByteArray txMessage);
    void ClosePort();
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);
    void handleRxTimeout();
};

#endif // SERIALWORKER_H
