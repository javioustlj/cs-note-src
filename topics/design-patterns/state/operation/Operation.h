#pragma once

#include "OperationStatus.hh"
#include "OperationType.hh"

class Operation
{
public:
    explicit Operation(OperationType type);

    virtual ~Operation();

    void setStatus(OperationStatus status);

    OperationType getType();
    OperationStatus getStatus();

private:
    OperationStatus     status;
    const OperationType type;
};
