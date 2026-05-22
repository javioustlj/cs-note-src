#include "RxState.hh"

RxState::RxState(const StateName enumName)
    : enumName(enumName)
{
}

RxState::~RxState()
{
}

StateName RxState::toEnum() const
{
    return enumName;
}

void RxState::onEntry()
{
}

void RxState::onExit()
{
}

void RxState::handleInterrupt()
{
    // do something
}
