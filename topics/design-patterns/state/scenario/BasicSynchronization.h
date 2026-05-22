#pragma once

#include "MasterScenario.hh"

class BasicSynchronization : public Scenario
{
public:
    BasicSynchronization();
    virtual ~BasicSynchronization() override {}

    void                                   hold();
    std::shared_ptr<operations::Operation> stop();
    std::shared_ptr<operations::Operation> handleEvent(const events::Event &);
    std::shared_ptr<operations::Operation> start(interface::RateMbps rate);
    std::shared_ptr<operations::Operation> switchTo(tx::StateName txState, rx::StateName rxState);
};
