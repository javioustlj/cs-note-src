#include <iostream>

void test()
{
    int a = 1;
    // const int b; 语法错误，b是一个常量，必须进行初始化
    const int b = 2; // OK
    const int * p1;  // OK，p1 是一个指针，指向一个int型常量
    int const * p2;  // OK, const int 和int const 相同，故同上
    // int * const p3; p3 是一个常量，故需要做初始化
    int * const p3 = &a; // p3 是一个常量指针，指向一个int
}

int main(void)
{
    test();
    return 0;
}
