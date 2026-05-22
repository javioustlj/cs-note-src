#pragma once

#include "Operation.h"

class NoOperation : public Operation
{
public:
    explicit NoOperation();
    virtual ~NoOperation() override;
};

