#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <cassert>
 
std::atomic<int> acnt = {0};
int cnt = 0;

void f_cnt()
{
    for (int n = 0; n < 1000; ++n)
        ++cnt;
}
void f_acnt()
{
    for (int n = 0; n < 1000; ++n)
        acnt.fetch_add(1, std::memory_order_relaxed);
}

std::atomic<std::string*> ptr;
int data;

void producer()
{
    std::string* p = new std::string("Hello");
    data = 42;
    ptr.store(p, std::memory_order_release);
}
 
void consumer()
{
    std::string* p2;
    while (!(p2 = ptr.load(std::memory_order_consume)));
    assert(*p2 == "Hello"); // 绝无出错： *p2 从 ptr 携带依赖
    assert(data == 42); // 可能出错，也可能不出错
}
 


void f_non_atomic()
{
    std::cout << "===========================================" << std::endl;
    std::cout << "Non Atomic" << std::endl;
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n)
        v.emplace_back(f_cnt);
    for (auto& t : v)
        t.join();

    // std::vector<std::jthread> pool;
    // for (int n = 0; n < 10; ++n)
    //     pool.emplace_back(f_cnt);
    std::cout << "最终计数器值为 " << cnt << '\n';
}

void f_relaxed()
{
    std::cout << "===========================================" << std::endl;
    std::cout << "MEMORY ORDER: Released" << std::endl;
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n)
        v.emplace_back(f_acnt);
    for (auto& t : v)
        t.join();
    std::cout << "最终计数器值为 " << acnt << '\n';
}


void f_consume()
{
    std::cout << "===========================================" << std::endl;
    std::cout << "MEMORY ORDER: Consume" << std::endl;
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join(); t2.join();
}
int main()
{
    f_non_atomic();
    f_relaxed();
    f_consume();
    return 0;
}
