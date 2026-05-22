#include <iostream>

void test()
{
    char *p1 = "hello";
    char *p2 = "hello";
    std::cout << (p1 == p2) << std::endl;
}


int main()
{
    test();
    return 0;
}
