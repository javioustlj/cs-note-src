#include <iostream>
#include <cstring>
using namespace std;

void fun1(char str[20])
{
    cout << "fun1 "<< sizeof(str) << endl;
}

void fun2(char a[10][9])
{
    cout << "fun2 " << sizeof(a) << endl;
}

int func3()
{
    cout << "hello world" << endl;
    return 0;
}

struct A
{};

struct B
{
  char c;
  int i;
};

struct C
{
  int i;
  char c;
};

struct D
{
  char c1;
  char c2;
  int i;
};

struct E
{
  int i;
  char c1;
  char c2;
};

struct F
{
  char c1;
  int i;
  char c2;
};

union G
{
  char c1;
  int i;
  char c2;
  double d;
};

class H
{};

class I
{
private:
  int i;
};

class J
{
public:
  virtual void display();
};

class K
{
private:
  int i;
public:
  void display();
};

class L
{
private:
  int i;
public:
  virtual void display();
};

class M
{
private:
  int i;
  int j;
public:
  virtual void display();
};

class N
{
private:
  static int i;
};

class O
{
private:
  static int i;
  int j;
};

enum P
{
    E1 = 0,
    E2 = 1,
    E3 = 1,
    E4 = 2
};
enum Q
{
};

void func4()
{
    cout << "====================================" << endl;
    cout << "复合类型" << endl;
    cout << "sizeof(A): " << sizeof(A) << endl; // 1  (编译器实现)
    cout << "sizeof(B): " << sizeof(B) << endl; // 8  (内存对齐)
    cout << "sizeof(C): " << sizeof(C) << endl; // 8  (内存对齐)
    cout << "sizeof(D): " << sizeof(D) << endl; // 8  (内存对齐)
    cout << "sizeof(E): " << sizeof(E) << endl; // 8  (内存对齐)
    cout << "sizeof(F): " << sizeof(F) << endl; // 12 (内存对齐)

    cout << "sizeof(G): " << sizeof(G) << endl; // 8  (内存共用)

    cout << "sizeof(H): " << sizeof(H) << endl; // 1   (编译器实现)
    cout << "sizeof(I): " << sizeof(I) << endl; // 4
    cout << "sizeof(J): " << sizeof(J) << endl; // 8   (虚指针)
    cout << "sizeof(K): " << sizeof(K) << endl; // 4
    cout << "sizeof(L): " << sizeof(L) << endl; // 16  (虚指针)  对比一下sizeof(M)
    cout << "sizeof(M): " << sizeof(M) << endl; // 16  (虚指针)  对比一下sizeof(L)

    cout << "sizeof(N): " << sizeof(N) << endl; // 1   (静态成员变量不专属某一对象)
    cout << "sizeof(O): " << sizeof(O) << endl; // 4   (静态成员变量不专属某一对象)
    cout << "sizeof(P): " << sizeof(P) << endl; // 4   (静态成员变量不专属某一对象)
    cout << "sizeof(Q): " << sizeof(Q) << endl; // 4   (静态成员变量不专属某一对象)
}


int main()
{
    cout << "====================================" << endl;
    cout << "sizeof(int) " << sizeof(int) << endl; // 4
    cout << "sizeof(long) " << sizeof(long) << endl; // 8

    cout << "====================================" << endl;
    cout << "sizeof 只看类型，不真的计算表达式" << endl;
    int i = 0;
    cout << sizeof(++i) << endl; // 4
    cout << i << endl;           // 0, 注意不是1哦
    cout << sizeof(i++) << endl;
    cout << i << endl;
    cout << sizeof(func3()) << endl; // 4, 注意没有hello world打印哈

    cout << "====================================" << endl;
    cout << "指针类型的大小是固定的，32位系统是4字节，64位系统是8字节" << endl;
    char *p1;
    cout << sizeof(p1) << endl; // 8
    int *p2 = new int[100];
    cout << sizeof(p2) << endl; // 8
    delete[] p2;
    float *p3;
    cout << sizeof(p3) << endl; // 8
    double ******p4;
    cout << sizeof(p4) << endl; // 8

    cout << "====================================" << endl;
    cout << "对于数组做sizeof那么，就是数组实际的占用空间" << endl;
    char str[100] = "abcdefg";
    fun1(str);                    // 8  (fun1中的形参str是指针变量)
    cout << sizeof(str) << endl;  // 100  (str的容量为100)
    cout << sizeof(*str) << endl; // 1    (*str是一个字符数据)
    cout << strlen(str) << endl;  // 7    (字符串str的长度)

    char str1[] = "abcdeofg";     //  (里面是字符'o')
    cout << sizeof(str1) << endl; // 9
    cout << strlen(str1) << endl; // 8

    char str2[] = "abcde0fg";     //  (里面是字符'0',不等于'\0')
    cout << sizeof(str2) << endl; // 9
    cout << strlen(str2) << endl; // 8

    char str3[] = "abcde\0fg";
    cout << sizeof(str3) << endl; // 9
    cout << strlen(str3) << endl; // 5

    char str4[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};
    cout << sizeof(str4) << endl; // 7
    cout << strlen(str4) << endl; // 7  (数值不确定)

    char str5[] = {'a', 'b', 'c', 'd', 'e', 'o', 'f', 'g'};
    cout << sizeof(str5) << endl; // 8
    cout << strlen(str5) << endl; // 13  (数值不确定)

    char str6[] = {'a', 'b', 'c', 'd', 'e', '0', 'f', 'g'};
    cout << sizeof(str6) << endl; // 8
    cout << strlen(str6) << endl; // 21  (数值不确定)

    char str7[] = {'a', 'b', 'c', 'd', 'e', '\0', 'f', 'g'};
    cout << sizeof(str7) << endl; // 8
    cout << strlen(str7) << endl; // 5

    char str8[] = {'a', 'b', 'c', 'd', 'e', 0, 'f', 'g'};
    cout << sizeof(str8) << endl; // 8
    cout << strlen(str8) << endl; // 5

    char str9[] = "";
    cout << sizeof(str9) << endl; // 1
    cout << strlen(str9) << endl; // 0
    char a[10][9];
    cout << sizeof(a) << endl; // 90
    fun2(a);                   // 8  （fun2中的a为指针变量）

    func4();

    return 0;
}
