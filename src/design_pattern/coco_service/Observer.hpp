#pragma once

#include <memory>
#include <set>

class IDisconnectObservers;

class Observer
{
public:
    Observer();
    virtual ~Observer();

    void attach(IDisconnectObservers&);
    void detach(IDisconnectObservers&);

private:
    Observer(const Observer&) = delete;
    Observer(Observer&&)      = delete;
    Observer& operator=(const Observer&) = delete;
    Observer& operator=(Observer&&) = delete;

    std::set<IDisconnectObservers*> signals;
};
