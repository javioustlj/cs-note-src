#pragma once

#include <list>
#include "ISubject.hpp"

class Subject : public ISubject {
 public:

  void attach(IObserver *observer) override;
  void detach(IObserver *observer) override;
  void notify() override;

  void updateBusinessMessage(const std::string& message);

 private:
  std::list<IObserver *> _list_observer;
  std::string _message;
};
