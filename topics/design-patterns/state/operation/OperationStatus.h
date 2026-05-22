#pragma once

#include <cstdint>

enum class OperationStatus : uint8_t
{
    Done,
    Ongoing,
    Pending,
    Failed
};
