#include <iostream>
#include "a.h"


extern int g_e;
void print()
{
    std::cout << "g_a: " << g_a << std::endl;
    // std::cout << "g_b: " << g_b << std::endl;
    // std::cout << "g_c: " << g_c << std::endl;
    std::cout << "g_d: " << g_d << std::endl;
    std::cout << "g_e: " << g_e << std::endl;
}

static void func2()
{
    std::cout << "static.cpp::func2" << std::endl;
    print();
    g_a = 100;
    g_d = 200;
    g_e = 300;
    print();
}

int main(void)
{
    print();
    func();
    // func2();
    print();
    func2();
    return 0;
}
