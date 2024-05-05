#pragma once

#include <cstdint>

enum class OperationType : uint8_t
{
    Unknown,
    Init,
    DeInit,
    SetTxState,
    ForceRxState,
    NotifyRxL1Sync,
    SwitchScenario,
    BcnCheckSync,
    StartTimer,
    StopTimer,
    AutoNegotiation,
    FifoFillLevelCheck,
    PerformSyncLossAction,
    NoOperation
};
