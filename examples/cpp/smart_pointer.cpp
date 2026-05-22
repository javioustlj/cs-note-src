#include <memory>
#include <iostream>

class A
{
public:
    A()
    {
        std::cout << "A::A()" << std::endl;
        func2();
    };
    virtual ~A() { std::cout << "A::~A()" << std::endl; };
    virtual void virtual_fun1() { std::cout <<"A::virtual_fun1()" << std::endl; };
    void func2() { std::cout <<"A::func2()" << std::endl; };
};

class B : public A
{
public:
    B() : a(0), b(0)
    {
        std::cout << "B::B()" << std::endl;
        func2();
    }
    B(int a, int b)
    {
        std::cout << "B::B(int a, int b)" << std::endl;
        func2();
    }
    ~B() { std::cout << "B::~B()" << std::endl; };
    virtual void virtual_fun1() { std::cout <<"B::virtual_fun1()" << std::endl; };
    void func2() { std::cout <<"B::func2()" << std::endl; };
private:
    int a;
    int b;
};


void test_shared_ptr()
{
    std::shared_ptr<A> a1;
    std::cout << "a1.use_count() = " << a1.use_count() << std::endl;
    std::shared_ptr<A> a2(new B);
    std::cout << "a2.use_count() = " << a2.use_count() << std::endl;
    std::shared_ptr<A> a3(a2);
    std::cout << "a2.use_count() = " << a2.use_count() << std::endl;
    std::cout << "a3.use_count() = " << a3.use_count() << std::endl;


    std::shared_ptr<A> a4 = std::make_shared<B>();
    std::shared_ptr<A> a5 = std::make_shared<B>(1, 2);

    A *a6 = new B();
    std::shared_ptr<A> a7(a6);
    std::cout << "a7.use_count() = " << a7.use_count() << std::endl;
    std::shared_ptr<A> a8(a6);
    std::cout << "a7.use_count() = " << a7.use_count() << std::endl;
    std::cout << "a7.use_count() = " << a8.use_count() << std::endl;
}

void test_unique_ptr()
{
    A *a1 = new B();
    std::unique_ptr<A> a2{a1};
    // std::unique_ptr<A> a3{a1};
    a2->virtual_fun1();
    // a3->virtual_fun1();
    std::unique_ptr<A> a4{std::move(a2)};
    //std::unique_ptr<A> a5{a2}; //编译不通过，因为拷贝构造函数被删除了
}
int main()
{
    test_unique_ptr();
    test_shared_ptr();
    return 0;
}
