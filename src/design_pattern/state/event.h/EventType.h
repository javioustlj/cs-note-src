#pragma once

#include <cstdint>

enum class EventType : uint8_t
{
    InternalTimer,
    RxStateChange,
    OperationFinished,
    PllStateChange,
    BcnSyncDone,
    FrameTimingChange
};

