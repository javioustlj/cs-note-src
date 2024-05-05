#pragma once

#include <cstdint>

enum class StateName : uint8_t
{
    Off      = 0,
    Idle     = 1,
    FrameTx  = 2,
    IdleReq  = 4,
    IdleAck  = 5,
    Disabled = 6,
};
