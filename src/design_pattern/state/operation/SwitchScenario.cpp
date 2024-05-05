#include "SwitchScenario.hh"


SwitchScenario::SwitchScenario(scenarios::ScenarioName scenarioName)
    : Operation(OperationType::SwitchScenario)
    , scenarioName(scenarioName)
{
}

SwitchScenario::~SwitchScenario()
{
}

scenarios::ScenarioName SwitchScenario::getScenario() const
{
    return scenarioName;
}

