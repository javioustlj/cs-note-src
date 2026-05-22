#include "a.h"
// g_a声明在头文件, 定义在a.cpp中
int g_a = 1;
// 静态全局变量g_c 直接定义在a.cpp中
static int g_c = 3;

// g_e 直接定义在a.cpp中，并未在头文件声明，但是在static.cpp中可以声明后使用
int g_e = 10;

void func()
{
    g_a = 4;
    // g_b = 5;
    g_c = 6;
    g_d = 8;
}

static void func2()
{
    g_a = 4;
    // g_b = 5;
    g_c = 6;
    g_d = 8;
}
