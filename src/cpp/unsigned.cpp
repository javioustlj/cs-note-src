#include <iostream>

void test()
{
    unsigned lo = 0, hi = 3;
    int a[4] = {0, 1, 2, 3};
    while ( ( lo < hi-- ) && ( 5 != a[hi] ) );
    std::cout << "lo = " << lo << " hi = " << hi << std::endl;

}
int main(void)
{
    test();
    return 0;
}
