#pragma once

#include <memory>

#include "coco/interface/RateMbps.hh"
#include "coco/port/portinterface/obsai/events/Event.hh"
#include "coco/port/portinterface/obsai/operations/Operation.hh"
#include "coco/port/portinterface/obsai/states/rx/StateName.hh"
#include "coco/port/portinterface/obsai/states/tx/StateName.hh"

class Scenario
{
public:
    virtual ~Scenario();
    virtual std::shared_ptr<operations::Operation> handleEvent(const events::Event &event) = 0;
    virtual std::shared_ptr<operations::Operation> start(interface::RateMbps rate) = 0;
    virtual std::shared_ptr<operations::Operation> stop() = 0;
    virtual std::shared_ptr<operations::Operation> switchTo(states::tx::StateName txState, states::rx::StateName rxState) = 0;
    virtual void hold() = 0;

protected:
    states::tx::StateName getTxState() const;
    states::rx::StateName getRxState() const;

    void setTxState(states::tx::StateName state);
    void setRxState(states::rx::StateName state);

private:
    states::tx::StateName txState;
    states::rx::StateName rxState;
};

