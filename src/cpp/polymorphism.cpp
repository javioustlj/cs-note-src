#include <iostream>

using namespace std;
/*
 *
 */


class A
{
public:
    A()
    {
        cout << "A::A()" << endl;
        virtual_fun1();
        func2();
    };
    virtual ~A() { cout << "A::~A()" << endl; };
    virtual void virtual_fun1() { cout <<"A::virtual_fun1()" << endl; };
    void func2() { cout <<"A::func2()" << endl; };
};

class B : public A
{
public:
    B() : a(0), b(0)
    {
        cout << "B::B()" << endl;
        virtual_fun1();
        func2();
    }
    ~B() { cout << "B::~B()" << endl; };
    virtual void virtual_fun1() { cout <<"B::virtual_fun1()" << endl; };
    void func2() { cout <<"B::func2()" << endl; };
private:
    int a;
    int b;
};

class Base
{
public:
    Base()
    {
        cout << "Base::Base()" << endl;
    };
    ~Base() { cout << "Base::~Base()" << endl; };
    virtual void virtual_fun1() { cout <<"Base::virtual_fun1()" << endl; };
    void func2() { cout <<"Base::func2()" << endl; };
};

class Derived : public Base
{
public:
    Derived() : a(0), b(0)
    {
        cout << "Derived::Derived()" << endl;
    }
    ~Derived() { cout << "Derived::~Derived()" << endl; };
    virtual void virtual_fun1() { cout <<"Derived::virtual_fun1()" << endl; };
    void func2() { cout <<"Derived::func2()" << endl; };
private:
    int a;
    int b;
};


/* 在存在虚函数的类中，虚函数指针占正常指针大小的字节
 * this指针不占用大小
 */
void test_sizeof_virtual_pointer()
{
    A* p = new B;
    B b;
    cout << "sizeof(A): " << sizeof(A) << endl;
    cout << "sizeof(b): " << sizeof(b) << endl;
    cout << "sizeof(B): " << sizeof(B) << endl;
    cout << "sizeof(p): " << sizeof(p) << endl;
    cout << "sizeof(*p): " << sizeof(*p) << endl;
    delete p;
}


void test_virtual_destructor()
{
    cout << "==================start test_virtual_destructor=============" << endl;
    Base *p1 = new Derived;
    delete p1;
    Derived *p2 = new Derived;
    delete p2;
    cout << "==================end test_virtual_destructor=============" << endl;
}


int main()
{
    test_sizeof_virtual_pointer();
    test_virtual_destructor();
    return 0;
}
