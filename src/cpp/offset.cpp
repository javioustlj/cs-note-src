#include <iostream>

struct AStruct {
    int a;
    int b;
    char c;
    double d;
};

#define OFFSET_OF(struct_type, member) \
    ((size_t)&(((struct_type *)0)->member))


template<typename T, typename U>
size_t offset(T& t, U T::* member) {
    return reinterpret_cast<size_t>(&(t.*member)) - reinterpret_cast<size_t>(&t);
}

void test()
{
    std::size_t size = OFFSET_OF(AStruct, b);
    std::cout << "The offset of b is: " << size << std::endl;
}

int main(void)
{
    test();
    return 0;
}
