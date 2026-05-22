#include <iostream>
#include "Observer.hpp"

Observer::Observer(ISubject &subject)
    : _subject(subject)
{
    _subject.attach(this);
}

Observer::~Observer()
{

}

void Observer::update(const std::string &message_from_subject)
{
    _message = message_from_subject;
    std::cout << _message << std::endl;
}

void Observer::removeMeFromTheList()
{
    _subject.detach(this);
}

