#include "WaitForAck.hh"

WaitForAck::WaitForAck()
    : RxState(StateName::WaitForAck)
{
}

WaitForAck::~WaitForAck()
{
}

void IdleAck::entry()
{
}

void IdleAck::exit()
{
}

void WaitForAck::handleInterrupt()
{
}
