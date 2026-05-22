#pragma once

#include "TxState.hh"

class IdleAck : public TxState
{
public:
    IdleAck();
    virtual ~IdleAck() override final;
    virtual entry() override final
    virtual exit() override final;
    virtual handleInterrupt() override final;
};
