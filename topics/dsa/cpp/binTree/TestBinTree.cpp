#include <iostream>
#include "BinTree.h"


void print_func(char c)
{
    std::cout << c << " ";
}

void test()
{
    BinTree<char> bt;
    std::cout << "Level Order " << std::endl;
    BinNodePosi<char> root = bt.insert('A');
    BinNodePosi<char> lc1 = bt.insert('B', root);
    BinNodePosi<char> rc1 = bt.insert(root, 'C');
    BinNodePosi<char> lc1_lc2 = bt.insert('D', lc1);
    BinNodePosi<char> lc1_rc2 = bt.insert(lc1, 'F');
    BinNodePosi<char> rc1_lc2 = bt.insert('G', rc1);
    BinNodePosi<char> rc1_rc2 = bt.insert(rc1, 'I');
    BinNodePosi<char> lc1_rc2_lc3 = bt.insert('E', lc1_rc2);
    BinNodePosi<char> rc1_lc2_rc3 = bt.insert(rc1_lc2, 'H');

    // std::cout << root->data << std::endl;
    // std::cout << root->lc->data << std::endl;
    // std::cout << root->rc->data << std::endl;
    // std::cout << lc1_lc2->data << std::endl;
    // std::cout << lc1_rc2->data << std::endl;
    // std::cout << rc1_lc2->data << std::endl;
    // std::cout << rc1_rc2->data << std::endl;
    // std::cout << lc1_rc2_lc3->data << std::endl;
    // std::cout << rc1_lc2_rc3->data << std::endl;
    std::cout << "Level Order 2" << std::endl;
    bt.travLevelOrder(print_func);
    std::cout << std::endl;
    std::cout << "Pre Order 2" << std::endl;
    bt.travPreOrder(print_func);
    std::cout << std::endl;
    std::cout << "In Order 2" << std::endl;
    bt.travInOrder(print_func);
    std::cout << std::endl;
    std::cout << "Post Order 2" << std::endl;
    bt.travPostOrder(print_func);
    std::cout << std::endl;

}

int main ( void )
{ //测试二叉树
    test();
   return 0;
}
