#pragma once

#include "StateName.hh"
#include "../State.hh"

class TxState : public State
{
public:
    TxState(const StateName enumName);
    virtual ~TxState() override;

    StateName getStateName() const;
    virtual void entry();
    virtual void exit();
    virtual void handleInterrupt();

private:
    const StateName enumName;
};
