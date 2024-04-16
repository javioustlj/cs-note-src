#include <iostream>

class A
{
public:
    A() : c(0)
    {
        std::cout << "A::A()" << std::endl;
        func2();
    };
    virtual void virtual_func1() { std::cout << "A::virtual_func1()" << std::endl;};
    void func2() { std::cout <<"A::func2()" << std::endl; };
    virtual void virtual_func3() { std::cout << "A::virtual_func3()" << std::endl;};
private:
    int c;
};


class B : public A
{
public:
    B() : a(0), b(0)
    {
        std::cout << "B::B()" << std::endl;
        func2();
    }
    B(int a, int b) : a(a), b(b)
    {
        std::cout << "B::B(int a, int b)" << std::endl;
        func2();
    }
    ~B() { std::cout << "B::~B()" << std::endl; };
    virtual void virtual_fun1() { std::cout <<"B::virtual_fun1()" << std::endl; };
    void func2() { std::cout <<"B::func2()" << std::endl; };
    virtual void virtual_func3() { std::cout << "B::virtual_func3()" << std::endl;};
private:
    int a;
    int b;
};

int main(void)
{
    A a1;
    A a2;
    B b1;
    B b2;

    return 0;
}
