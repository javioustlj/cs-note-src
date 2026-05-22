#pragma once

#include <cstdint>
#include "EventType.hh"

class Event
{
public:
    explicit Event(EventType type)
        : type(type)
    {
    }

    EventType getType() const
    {
        return type;
    }

private:
    const EventType type;
};
