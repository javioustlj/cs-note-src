#pragma once

#include "Event.hh"
#include "EventType.hh"

class InternalTimer : public Event
{
public:
    explicit InternalTimer(const ObsaiTimer timer);
    ObsaiTimer getTimer() const;

private:
    ObsaiTimer timer;
};
