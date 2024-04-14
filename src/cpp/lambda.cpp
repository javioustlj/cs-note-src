/******************************************************************************************
 * Computer Science Note Source code
 * Javious Tian, javioustlj@outlook.com
 * 夏田墨  javious_tian@zuaa.zju.edu.cn
 * Copyright (c) 2024 Javioustlj. All rights reserved.
 ******************************************************************************************/


#include <algorithm>
#include <iostream>
#include <vector>


void test2()
{

    int a = 1, b = 1, c = 1;
    // int a, b, c;
    auto m1 = [a, &b, &c]() mutable
    {
        auto m2 = [a, b, &c]() mutable
        {
            std::cout << a << b << c << '\n';
            a = 4; b = 4; c = 4;
        };
        a = 3; b = 3; c = 3;
        m2();
    };

    a = 2; b = 2; c = 2;
    std::cout << a << b << c << '\n';
    m1();                             // calls m2() and prints 123
    std::cout << a << b << c << '\n'; // prints 234
}

void test()
{
    std::vector<int> v{3, -4, 2, -8, 15, 267};
 
    auto print = [](const int& n) { std::cout << n << ' '; };
 
    std::cout << "before:\t";
    std::for_each(v.cbegin(), v.cend(), print);
    std::cout << '\n';
 
    // increment elements in-place
    std::for_each(v.begin(), v.end(), [](int &n) { n++; });
 
    std::cout << "after:\t";
    std::for_each(v.cbegin(), v.cend(), print);
    std::cout << '\n';
 
    struct Sum
    {
        void operator()(int n) { sum += n; }
        int sum {0};
    };
 
    // invoke Sum::operator() for each element
    Sum s = std::for_each(v.cbegin(), v.cend(), Sum());    
    std::cout << "sum:\t" << s.sum << '\n';
}

int main()
{
    // test();
    test2();
    // test3();
    return 0;
}
