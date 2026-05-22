#include <iostream>
#include "Subject.hpp"

void Subject::attach(IObserver *observer)
{
    _list_observer.push_back(observer);
}

void Subject::detach(IObserver *observer)
{
    _list_observer.remove(observer);
}

void Subject::notify()
{
    for (auto &observer : _list_observer)
    {
        observer->update(_message);
    }
}

void Subject::updateBusinessMessage(const std::string &message)
{
    _message = message;
    std::cout << "There are " << _list_observer.size() << " observers notified" << std::endl;
    notify();
}
