#include <iostream>
#include <thread>
#include "Singleton.hpp"

void Thread1(){
    // Following code emulates slow initialization.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    const Singleton& singleton = Singleton::getInstance(1);
    std::cout << singleton.getValue() << "\n";
}

void Thread2(){
    // Following code emulates slow initialization.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    const Singleton& singleton = Singleton::getInstance(2);
    std::cout << singleton.getValue() << "\n";
}

int main()
{
    std::cout <<"If you see the same value, then singleton was reused (yay!\n" <<
                "If you see different values, then 2 singletons were created (booo!!)\n\n" <<
                "RESULT:\n";   
    std::thread t1(Thread1);
    std::thread t2(Thread2);
    t1.join();
    t2.join();

    return 0;
}
