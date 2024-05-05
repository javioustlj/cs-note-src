#include "Scenario.hh"

Scenario::~Scenario()
{
}

states::tx::StateName Scenario::getTxState() const
{
    return txState;
}

states::rx::StateName Scenario::getRxState() const
{
    return rxState;
}

void Scenario::setTxState(states::tx::StateName state)
{
    txState = state;
}

void Scenario::setRxState(states::rx::StateName state)
{
    rxState = state;
}
