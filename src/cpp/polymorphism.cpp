#include <iostream>

using namespace std;

class A
{
    public:
        virtual ~A() { cout << "A::~A()" << endl; };
        void disp()
        {
            cout <<"A disp" << endl;
        }
};

class B : public A
{
public:
    ~B() { cout << "B::~B()" << endl; };
    void disp()
    {
        cout <<"B disp" << endl;
    }
private:
    int a;
    int b;
};

int main()
{
    A* p = new B;
    // p->disp();
    B b;
    cout << "sizeof(A): " << sizeof(A) << endl;
    cout << "sizeof(b): " << sizeof(b) << endl;
    cout << "sizeof(B): " << sizeof(B) << endl;
    cout << "sizeof(p)" << sizeof(p) << endl;
    delete p;
    return 0;
}
