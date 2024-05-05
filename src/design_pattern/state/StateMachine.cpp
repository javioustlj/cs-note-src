#include "StateMachine.hh"

StateMachine::StateMachine()
{
}

StateMachine::~StateMachine()
{
}

const std::shared_ptr<State> StateMachine::getCurrentState() const
{
    return currentState;
}

void StateMachine::makeTransitionTo(std::shared_ptr<State> newState)
{
    if (currentState)
        currentState->exit();

    currentState = newState;
    currentState->entry();
}
