#include "Subject.hpp"
#include "Observer.hpp"

void ClientCode() {
  Subject *subject = new Subject;
  Observer *observer1 = new Observer(*subject);
  Observer *observer2 = new Observer(*subject);
  Observer *observer3 = new Observer(*subject);
  Observer *observer4;
  Observer *observer5;

  subject->updateBusinessMessage("Hello World! :D");
  observer3->removeMeFromTheList();

  subject->updateBusinessMessage("The weather is hot today! :p");
  observer4 = new Observer(*subject);

  observer2->removeMeFromTheList();
  observer5 = new Observer(*subject);

  subject->updateBusinessMessage("My new car is great! ;)");
  observer5->removeMeFromTheList();

  observer4->removeMeFromTheList();
  observer1->removeMeFromTheList();

  delete observer5;
  delete observer4;
  delete observer3;
  delete observer2;
  delete observer1;
  delete subject;
}

int main() {
  ClientCode();
  return 0;
}
