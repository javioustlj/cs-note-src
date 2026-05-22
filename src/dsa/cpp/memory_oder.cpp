#include <cstdio>
#include <thread>
#include <atomic>

std::atomic<int> x(0), y(0);
int r1 = 0, r2 = 0;

void f1()
{
    int r1 = y.load(std::memory_order_relaxed);
    x.store(r1);
    // std::printf("f1: x = %d, y = %d, r1 = %d\n", x.load(), y.load(), r1);
}

void f2()
{
    int r2 = x.load(std::memory_order_relaxed);
    y.store(42, std::memory_order_relaxed);
    // std::printf("f1: x = %d, y = %d, r2 = %d\n", x.load(), y.load(), r2);
}


int main()
{
    // for (;r1 != 42 || r2 != 42;)
    // {
        std::thread t1(f1);
        std::thread t2(f2);
        t1.join();
        t2.join();
        std::printf("f1: x = %d, y = %d, r1 = %d, r2 = %d\n", x.load(), y.load(), r1, r2);
    // }

}
