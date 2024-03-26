#include "Observer.hh"
#include <algorithm>
#include "IDisconnectObservers.hh"

Observer::Observer()
{
}

Observer::~Observer()
{
    while (!signals.empty())
        detach(**signals.begin());
}

void Observer::attach(IDisconnectObservers& sig)
{
    signals.insert(&sig);
}

void Observer::detach(IDisconnectObservers& obs)
{
    auto address = std::addressof(obs);
    auto s       = signals.find(address);
    if (s != signals.end())
    {
        auto* sp = *s;
        signals.erase(s);
        sp->disconnect(*this);
    }
}
