#pragma once

using Rank = unsigned int; //秩

template <typename T> struct ListNode;
template <typename T> using ListNodePosi = ListNode<T>*; //列表节点位置

// 双向列表的节点
template <typename T> struct ListNode
{
    // 构造函数
    ListNode(); //针对header和trailer的构造
    ListNode ( const T &e, ListNodePosi<T> p = nullptr, ListNodePosi<T> s = nullptr );
    // 操作接口
    ListNodePosi<T> insertAsPred( const T &e ); //紧靠当前节点之前插入新节点
    ListNodePosi<T> insertAsSucc( const T &e ); //紧随当前节点之后插入新节点
    // 节点数据
    ListNodePosi<T> pred;
    ListNodePosi<T> succ;
    T data;
};

#include "ListNodeImplementation.h"
