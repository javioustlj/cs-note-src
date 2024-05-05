#pragma once

#include <cstdint>

enum class StateName : uint8_t
{
    Unsync            = 0,
    WaitForK287Idles  = 1,
    WaitForFrameSyncT = 2,
    FrameSync         = 3,
    WaitForSeed       = 4,
    WaitForAck        = 5,
    Disabled          = 6,
};
