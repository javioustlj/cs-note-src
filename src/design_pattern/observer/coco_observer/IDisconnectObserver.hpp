#pragma once

class Observer;

class IDisconnectObservers
{
public:
    virtual ~IDisconnectObserver() = default;
    virtual void disconnect(Observer& observer) = 0;
};
