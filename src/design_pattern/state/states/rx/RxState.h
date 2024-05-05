#pragma once

#include "StateName.hh"
#include "coco/port/portinterface/obsai/State.hh"

class RxState : public State
{
public:
    RxState(const StateName enumName);
    virtual ~RxState() override;

    StateName getStateName() const;
    virtual void onEntry();
    virtual void onExit();
    virtual void handleInterrupt() const;

private:
    const StateName enumName;
};
