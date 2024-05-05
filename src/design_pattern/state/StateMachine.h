#pragma once

#include <memory>
#include "State.h"

class StateMachine
{
public:
    StateMachine();
    virtual ~StateMachine();

    virtual void init() = 0;
    virtual void deinit() = 0;

protected:
    const std::shared_ptr<State> getStateObject() const;
    void makeTransitionTo(std::shared_ptr<State>);

private:
    std::shared_ptr<State> currentState;
};

