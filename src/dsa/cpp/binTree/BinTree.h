#pragma once

#include "BinNode.h"
#include <iostream>

template <typename T>
class BinTree
{
public:
    BinTree();
    ~BinTree();
    Rank size() const;      //规模, BinNode 的个数
    bool empty() const;
    BinNodePosi<T> root() const;
    BinNodePosi<T> insert( T const& );                 //插入根节点
    BinNodePosi<T> insert( T const&, BinNodePosi<T> ); //插入左孩子
    BinNodePosi<T> insert( BinNodePosi<T>, T const& ); //插入右孩子
    BinNodePosi<T> attach( BinTree<T>*&, BinNodePosi<T> );  //接入左子树
    BinNodePosi<T> attach( BinNodePosi<T>, BinTree<T>*& );  //接入右子树
    Rank remove ( BinNodePosi<T> ); //子树删除
    BinTree<T>* secede ( BinNodePosi<T> ); //子树分离
    template <typename VST>
    void travLevelOrder( VST& visit );
    template <typename VST>
    void travPreOrder( VST& visit );
    template <typename VST>
    void travInOrder( VST& visit );
    template <typename VST> 
    void travPostOrder( VST& visit );

protected:
   Rank _size;
   BinNodePosi<T> _root;
};

#include "BinTreeImplementation.h"
