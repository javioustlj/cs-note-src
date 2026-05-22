// 声明在头文件的外部链接变量
extern int g_a;
// 定义在头文件的全局变量, 会导致重定义错误
// int g_b;
// 声明在头文件中的静态变量，不符合语法
// extern int g_c;
// 定义在头文件中的静态全局变量，符合语法，但是通常不这么做
static int g_d;

void func();
static void func2();
