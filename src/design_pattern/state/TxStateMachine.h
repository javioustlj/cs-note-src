#pragma once

#include <memory>

#include "StateMachine.hh"
#include "states/tx/TxState.hh"


using StateCollection = std::map<StateName, std::shared_ptr<TxState>>;

class TxStateMachine : public StateMachine
{
public:
    TxStateMachine();
    ~TxStateMachine() override;

    void init();
    void deinit();
    states::tx::StateName getState() const;
    void setState(states::tx::StateName stateName);
    void handleInterrupt();

private:
    states::tx::StateCollection stateCollection;
};
