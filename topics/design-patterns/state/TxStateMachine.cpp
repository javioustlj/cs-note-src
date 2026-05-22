#include "TxStateMachine.hh"
#include "state/tx/IdleAck.h"


TxStateMachine::TxStateMachine()
{
    stateCollection.insert({StateName::IdleAck, std::make_shared<IdleAck>()});
    makeTransitionTo(stateCollection[tx::StateName::Off]);
}

TxStateMachine::~TxStateMachine()
{
}

void TxStateMachine::init(interface::RateMbps rate)
{

}

void TxStateMachine::deinit()
{

}

states::tx::StateName TxStateMachine::getState() const
{

}

void TxStateMachine::setState(states::tx::StateName stateName)
{

}

void TxStateMachine::handleInterrupt()
{
    // handleInterrupt()
    // This may cause
}
