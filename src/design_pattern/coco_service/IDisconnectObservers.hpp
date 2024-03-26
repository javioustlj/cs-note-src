#pragma once

class Observer;

class IDisconnectObservers
{
public:
    virtual ~IDisconnectObservers()    = default;
    virtual void disconnect(Observer&) = 0;
};
