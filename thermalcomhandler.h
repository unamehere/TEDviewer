#ifndef THERMALCOMHANDLER_H
#define THERMALCOMHANDLER_H

#include <QObject>

class thermalComHandler : public QObject
{
    Q_OBJECT
public:
    explicit thermalComHandler(QObject *parent = nullptr);

signals:

public slots:
};

#endif // THERMALCOMHANDLER_H
