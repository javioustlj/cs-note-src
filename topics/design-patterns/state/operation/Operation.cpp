#include "Operation.h"

explicit Operation::Operation(OperationType type)
    : status(OperationStatus::Pending)
    , type(type)
{
}

virtual Operation::~Operation()
{
}

void Operation::setStatus(OperationStatus status)
{
    this->status = status;
}

OperationType Operation::getType()
{
    return type;
}

OperationStatus Operation::getStatus()
{
    return status;
}
