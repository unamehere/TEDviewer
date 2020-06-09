#ifndef COMMAND_H
#define COMMAND_H
#include <QObject>

const QStringList writeCalls = {"GR", "GT", "MR", "MT", "MM"};

class command
{
    QString comm;
    unsigned deg;
    unsigned xPos;
    unsigned yPos;
public:
    explicit command(int ctComm, unsigned deg_ = 0,unsigned xPos_ = 0,unsigned yPos_ = 0);

    const QString& getComm() const
    {
        return comm;
    }
    const unsigned& getDeg() const
    {
        return  deg;
    }
    unsigned getXPos() const;
    unsigned getYPos() const;
};

#endif // COMMAND_H
