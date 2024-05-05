#pragma once

#include "Event.hh"
#include "coco/port/portinterface/obsai/operations/Operation.hh"

class OperationFinished : public Event
{
public:
    OperationFinished(operations::OperationType operationType, operations::OperationStatus operationStatus);
    operations::OperationType   getOperationType() const;
    operations::OperationStatus getOperationStatus() const;

private:
    operations::OperationType   operationType;
    operations::OperationStatus operationStatus;
};

