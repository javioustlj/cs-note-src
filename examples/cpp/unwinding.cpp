#include <iostream>
#include <string>
#include <memory>

using namespace std; 

class A
{
public:
    A (int a) : a(a) { cout << "A::A(): " << a << endl; };
    virtual ~A() { cout << "A::~A(): " << a << endl; };
    void disp()
    {
        cout <<"A disp" << endl;
    }
private:
    int a;
};

class B : public A
{
public:
    B (int a, const string &b) : A(a), b(b) { cout << "B::B(): " << b << endl; };
    ~B() { cout << "B::~B(): " << b << endl; };
    void disp()
    {
        cout <<"B disp" << endl;
    }
private:
    int a;
    string b;
};

void f1()
{
    cout << "f1() Start " << endl; 
    B b1(1, "stack object");
    [[maybe_unused]] A *p1 = new B(2, "heap object");
    shared_ptr<A> p2 = make_shared<B>(3, "shared ptr object");

	throw 100; 
	cout << "f1() End " << endl; 
}

void f2()
{ 
	cout << "f2() Start " << endl; 
	f1(); 
	cout << "f2() End " << endl; 
}

void f3() 
{ 
	cout << "f3() Start " << endl; 
	try { 
		f2();
	} 
	catch (int i) { 
		cout << "Caught Exception: " << i << endl; 
	} 
	cout << "f3() End" << endl;
}


int main()
{
	f3();
	return 0;
}
