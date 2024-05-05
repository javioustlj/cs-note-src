#pragma once

#include "ConnipInterruptEvent.hh"
#include "StateCollaborators.hh"
#include "StateMachine.hh"
#include "coco/port/portinterface/obsai/states/rx/RxState.hh"
#include "coco/port/portinterface/obsai/states/rx/StateCollection.hh"
#include <lola/obsai/definitions/ObsaiEvent.hpp>
#include "coco/utils/Observer.hh"
#include "coco/utils/SignalToMany.hh"
#include "coco/interface/RateMbps.hh"


class RxStateMachine : public StateMachine
{
public:
    RxStateMachine();
    ~RxStateMachine() override;

    void init(interface::RateMbps rate);
    void deinit();
    states::rx::StateName getState() const;
    void setState(rx::StateName stateName);
    void handleInterrupt();

private:
    states::rx::StateCollection stateCollection;
};

