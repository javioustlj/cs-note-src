#include "SyncSupervisor.hh"
#include <thread>


namespace coco {
namespace port {
namespace portinterface {
namespace obsai {

SyncSupervisor::SyncSupervisor(resources::Resources& resources, scenarios::ScenarioCollection collection,
                               std::shared_ptr<RxStateMachine> rxStateMachine,
                               std::shared_ptr<TxStateMachine> txStateMachine,
                               std::shared_ptr<ObsaiPortStatusProvider> obsaiStatusProvider,
                               std::shared_ptr<ObsaiPortStateProvider> obsaiStateProvider)
    : currentScenarioName(scenarios::ScenarioName::Unknown)
    , collection(collection)
    , rxStateMachine(rxStateMachine)
    , txStateMachine(txStateMachine)
{

}

void SyncSupervisor::onHandleMessageFun1()
{
    // 判断当前状态是什么，以采取不同的策略；
    events::RxStateChange event(stateName);
    handleEvent(event);
}


// Scenario的切换
void SyncSupervisor::setScenario(const scenarios::ScenarioName scenarioName)
{
    stopCurrentScenario();
    mRateIndex = mPreferredRateIndex;
    currentScenarioName = scenarioName;
    supervisorActive = true;
    notifyStatusProvider();
    scenario = collection[scenarioName];
    startScenario(scenario);
}

void SyncSupervisor::switchScenario(const scenarios::ScenarioName scenarioName)
{
    stopCurrentScenario();
    currentScenarioName = scenarioName;
    scenario = collection[scenarioName];
    std::shared_ptr<operations::Operation> op = scenario->switchTo(txStateMachine->getState(), rxStateMachine->getState());
    if (op)
        executeOperation(op);
}

void SyncSupervisor::startScenario(std::shared_ptr<scenarios::Scenario> sc)
{
    std::shared_ptr<operations::Operation> op = sc->start(mRates[mRateIndex]);
    if (op)
        executeOperation(op);
}

void SyncSupervisor::stopCurrentScenario()
{
    if (scenario)
    {
        std::shared_ptr<operations::Operation> op = scenario->stop();
        if (op)
            executeOperation(op);
    }
}

void SyncSupervisor::stop()
{
    if (supervisorActive)
    {
        txStateMachine->deinit();
        rxStateMachine->deinit();
        stopCurrentScenario();
    }
    supervisorActive = false;
    notifyStatusProvider();
}

bool SyncSupervisor::isSupervisorActive()
{
    return supervisorActive;
}


void SyncSupervisor::executeOperation(const std::shared_ptr<operations::Operation> operation)
{
    bool isOperationValid = true;

    if (!isSupervisorActive())
        return;

    switch (operation->getType())
    {
    case operations::OperationType::PerformSyncLossAction:
    case operations::OperationType::Init:
    case operations::OperationType::AutoNegotiation:
    case operations::OperationType::SetTxState:
    case operations::OperationType::StartTimer:
    case operations::OperationType::StopTimer:
    case operations::OperationType::ForceRxState:
    case operations::OperationType::NotifyRxL1Sync:
    case operations::OperationType::SwitchScenario:
    case operations::OperationType::FifoFillLevelCheck:
    case operations::OperationType::DeInit:
    break;

    default:
        isOperationValid = false;
        break;
    }

    if (isOperationValid)
    {
        onOperationFinished(operation);
    }
}

void SyncSupervisor::handleEvent(const events::Event& event)
{
    executeOperation(scenario->handleEvent(event));
}

void SyncSupervisor::notifyStatusProvider()
{
    // notification to others
}

void SyncSupervisor::onOperationFinished(const std::shared_ptr<operations::Operation> operation)
{
    events::OperationFinished event(operation->getType(), operation->getStatus());
    handleEvent(event);
}
