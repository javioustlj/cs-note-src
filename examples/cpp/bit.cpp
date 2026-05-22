#include <iostream>
#include <cstdint>

void test()
{
    std::uint32_t a =  0x0000000A; // 0000 0000 0000 0000 0000 0000 0000 1010
    std::int32_t b = -10;
    std::int32_t c = 10;
    std::cout << std::hex << std::uppercase << a << std::endl;
    std::cout << std::hex << std::uppercase << b << std::endl;
    std::cout << std::hex << std::uppercase << c << std::endl;

    std::cout << std::hex << std::uppercase << (a << 1) << std::endl;
    std::cout << std::hex << std::uppercase << (b << 1) << std::endl;
    std::cout << std::hex << std::uppercase << (c << 1) << std::endl;

    std::cout << std::dec << (b << 1) << std::endl;

}

int main(void)
{
    test();
    return 0;
}
