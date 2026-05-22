#include <iostream>
using namespace std;


// 父类（基类）
class Parent {
public:
    static void (Parent::*showPointer)();
    // 虚函数
    virtual void show() {
        cout << "我是父类的show函数" << endl;
    }

    void show_wrap() {
        (this->*showPointer)();
    }

    // 虚析构函数（重点！）
    virtual ~Parent() {}
};

// 子类（派生类）
class Child : public Parent {
public:
    // 重写（override）父类的虚函数（virtual可省略，建议加上override关键字显式声明）
    void show() override {
        cout << "我是子类的show函数" << endl;
    }
};

void (Parent::*Parent::showPointer)() = &Parent::show;

int main() {
    Parent* p1 = new Parent();
    Parent* p2 = new Child(); // 父类指针指向子类对象
    Child* p3 = new Child();

    // p1->show(); // 输出：我是父类的show函数（对象是Parent）
    // p2->show(); // 输出：我是子类的show函数（对象是Child，动态绑定）
    // p3->show(); // 输出：我是子类的show函数（对象是Child）
    p2->show_wrap();

    delete p1;
    delete p2; // 虚析构保证子类析构函数被调用
    delete p3;
    return 0;
}
