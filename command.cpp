#include "command.h"

unsigned command::getXPos() const
{
    return xPos;
}

unsigned command::getYPos() const
{
    return yPos;
}

command::command(int ctComm, unsigned deg_ ,unsigned xPos_,unsigned yPos_):xPos(xPos_), yPos(yPos_)
{
    deg = deg_;
    comm = writeCalls[ctComm];
}
