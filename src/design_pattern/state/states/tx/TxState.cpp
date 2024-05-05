#include "TxState.hh"

TxState::TxState(const StateName enumName)
    : enumName(enumName)
{
}

TxState::~TxState()
{
}

StateName TxState::getStateName() const
{
    return enumName;
}

void TxState::entry()
{
    // something related to RX
}

void TxState::exit()
{
    // something related to TX
}

void TxState::handleInterrupt()
{
    // do something
}
