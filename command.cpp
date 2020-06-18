#include "command.h"

const QStringList writeCalls = {"GR", "GT", "MR", "MT", "MM", "OK", "ER"};

unsigned command::getXPos() const
{
    return xPos;
}

unsigned command::getYPos() const
{
    return yPos;
}

QString command::getCommandStr(int callID)
{
    QString retStr = "";
    if(callID >0 && callID < writeCalls.length())
    {
        retStr = writeCalls[callID];
    }
    return retStr;
}

command::command(int ctComm, unsigned deg_ ,unsigned xPos_,unsigned yPos_):xPos(xPos_), yPos(yPos_)
{
    deg = deg_;
    comm = writeCalls[ctComm];
}
