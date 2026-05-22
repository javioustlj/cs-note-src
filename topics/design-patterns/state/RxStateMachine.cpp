#include "RxStateMachine.hh"
#include "states/rx/StateName.hh"

RxStateMachine::RxStateMachine()
{
    stateCollection.insert({rx::StateName::WaitForAck, std::make_shared<rx::WaitForAck>()});
    makeTransitionTo(stateCollection[states::rx::StateName::Unsync]);
}

RxStateMachine::~RxStateMachine()
{
}

void RxStateMachine::init(interface::RateMbps rate)
{

}

void RxStateMachine::deinit()
{

}

states::rx::StateName RxStateMachine::getState() const
{

}

void RxStateMachine::setState(states::rx::StateName stateName)
{

}

void RxStateMachine::handleInterrupt()
{

}
