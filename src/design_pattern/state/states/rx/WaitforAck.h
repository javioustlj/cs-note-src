#pragma once

#include "RxState.hh"

class WaitForAck : public RxState
{
public:
    WaitForAck();
    virtual ~WaitForAck() override final;

    virtual entry() override final
    virtual exit() override final;
    virtual handleInterrupt() override final;

};
