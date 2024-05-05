#include "OperationFinished.hh"

namespace coco {
namespace port {
namespace portinterface {
namespace obsai {
namespace events {

OperationFinished::OperationFinished(operations::OperationType   operationType,
                                     operations::OperationStatus operationStatus)
    : Event(EventType::OperationFinished)
    , operationType(operationType)
    , operationStatus(operationStatus)
{
}

operations::OperationType OperationFinished::getOperationType() const
{
    return operationType;
}

operations::OperationStatus OperationFinished::getOperationStatus() const
{
    return operationStatus;
}

} /* namespace events */
} /* namespace obsai */
} /* namespace portinterface */
} /* namespace portcontrol */
} /* namespace coco */
