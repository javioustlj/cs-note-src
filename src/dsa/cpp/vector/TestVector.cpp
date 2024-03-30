#include <iostream>
#include "Vector.h"

void test()
{
    int a[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Vector<int> v2(a, 9);
    v2.traverse([](int& e) { std::cout << e << std::endl;});
}
int main()
{
    test();
    return 0;
}
