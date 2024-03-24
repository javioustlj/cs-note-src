#pragma once

#include "ISubject.hpp"

class Observer : public IObserver
{
public:
    Observer(ISubject &subject);
    virtual ~Observer();
    void update(const std::string &message_from_subject) override;
    void removeMeFromTheList();

private:
    std::string _message;
    ISubject &_subject;
};
