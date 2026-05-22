#include "InternalTimer.hh"

InternalTimer::InternalTimer(const ObsaiTimer timer)
    : Event(EventType::InternalTimer)
    , timer(timer)
{
}

ObsaiTimer InternalTimer::getTimer() const
{
    return timer;
}
