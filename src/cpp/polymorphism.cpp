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

int main()
{
    test_sizeof_virtual_pointer();
    return 0;
}
