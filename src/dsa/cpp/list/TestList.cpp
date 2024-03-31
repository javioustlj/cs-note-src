#include <iostream>
#include "List.h"

void traverse_func( int &x)
{
    std::cout << "traverse_func: " << x << std::endl;
}

void test()
{
    List<int> list;
    ListNodePosi<int> p = list.insertAsFirst(1);
    p = list.insert(p, 2);
    p = list.insert(p, 3);
    list.traverse(traverse_func);
}

int main()
{
    test();
    return 0;
}
