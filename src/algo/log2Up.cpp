#include <iostream>

template <typename Int>
inline int log2Up(Int x)
{
    int w, n;
    if (x != 0)
        x -= 1;

    if (x == 0)
    {
        return 0;
    }
    else if (sizeof(Int) <= sizeof(unsigned int))
    {
        w = 8 * sizeof(unsigned int);
        n = __builtin_clz((unsigned int)x);
    }
    else if (sizeof(Int) <= sizeof(unsigned long))
    {
        w = 8 * sizeof(unsigned long);
        n = __builtin_clzl((unsigned long)x);
    }
    else if (sizeof(Int) <= sizeof(unsigned long long))
    {
        w = 8 * sizeof(unsigned long long);
        n = __builtin_clzll((unsigned long long)x);
    }
    else
    {
        static_assert(sizeof(Int) <= sizeof(unsigned long long), "Unsupported integer size.");
    }

    return w - n;
}

void test()
{
    for (int i = 0; i < 20; ++i)
    {
        std::cout << i << " : " << log2Up(i) << " : " << ( 1 << log2Up(i))<< std::endl;
    }

}

int main()
{
    test();
    return 0;
}
