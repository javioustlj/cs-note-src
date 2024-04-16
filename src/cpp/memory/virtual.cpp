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


int main(void)
{
    A a1;
    A a2;

    return 0;
}
