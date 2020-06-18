#ifndef COMMAND_H
#define COMMAND_H
#include <QObject>

class command
{
    QString comm;
    unsigned deg;
    unsigned xPos;
    unsigned yPos;
public:
    explicit command(int ctComm, unsigned deg_ = 0,unsigned xPos_ = 0,unsigned yPos_ = 0);

    static QString getCommandStr(int callID);

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
