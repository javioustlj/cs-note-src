#include "BasicSynchronization.hh"

BasicSynchronization::BasicSynchronization()
{
}

std::shared_ptr<operations::Operation> BasicSynchronization::stop()
{
    // do something
    // return a operation, it might stop a timer.
    return std::make_shared<operations::NoOperation>();
}

void BasicSynchronization::hold()
{
    // do something
    // then return nothing.
}

std::shared_ptr<operations::Operation> BasicSynchronization::start(interface::RateMbps rate)
{
    // do something
    // then return a operation to start a timer
    return std::make_shared<operations::NoOperation>();
}

std::shared_ptr<operations::Operation> BasicSynchronization::switchTo(states::tx::StateName txState, states::rx::StateName rxState)
{
    // do something also start a new timer
    return std::make_shared<operations::NoOperation>();
}

std::shared_ptr<operations::Operation> BasicSynchronization::handleEvent(const events::Event& event)
{
    // handle event. different event, different process, different return.
    // switch (event.getType())
    // {
    //     case events::EventType::RxStateChange:
    //     case events::EventType::OperationFinished:
    //     case events::EventType::InternalTimer:
    //     case events::EventType::BcnSyncDone:
    //     case events::EventType::PllStateChange:
    //     case events::EventType::FrameTimingChange:
    //     default :
    //         return std::make_shared<operations::NoOperation>();
    // }
    return std::make_shared<operations::NoOperation>()
}
