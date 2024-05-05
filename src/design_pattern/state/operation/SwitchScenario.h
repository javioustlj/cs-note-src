#pragma once

#include "Operation.h"
#include "coco/port/portinterface/obsai/scenarios/ScenarioName.hh"

class OperationSwitchScenario : public Operation
{
public:
    explicit SwitchScenario(scenarios::ScenarioName scenarioName);
    virtual ~SwitchScenario() override;
    scenarios::ScenarioName getScenario() const;

private:
    scenarios::ScenarioName scenarioName;
};
