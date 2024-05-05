#pragma once

#include <memory>

class SyncSupervisor
{
public:
    SyncSupervisor(scenarios::ScenarioCollection collection,
                   std::shared_ptr<RxStateMachine> rxStateMachine,
                   std::shared_ptr<TxStateMachine> txStateMachine);

    void onOperationFinished(const std::shared_ptr<Operation> operation);
    void setScenario(const ScenarioName scenarioName);
    void switchScenario(const ScenarioName scenarioName);
    void stopCurrentScenario();
    void stop();
    void executeOperation(const std::shared_ptr<operations::Operation>);
    void handleEvent(const events::Event &event);
    bool isSupervisorActive();

private:
    void startScenario(std::shared_ptr<scenarios::Scenario> sc);

    scenarios::ScenarioName currentScenarioName;
    scenarios::ScenarioCollection collection;

    std::shared_ptr<scenarios::Scenario>      scenario;
    std::shared_ptr<RxStateMachine>           rxStateMachine;
    std::shared_ptr<TxStateMachine>           txStateMachine;
    bool                                      supervisorActive;
};
