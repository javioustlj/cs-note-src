#pragma once

#include <cassert>

template <typename T>
ListNode<T>::ListNode()
    : pred(nullptr)
    , succ(nullptr)
    , data(T())
{
}

template <typename T>
ListNode<T>::ListNode(const T& e, ListNode<T>* p, ListNode<T>* s)
    : pred(p)
    , succ(s)
    , data(e)
{
}

template <typename T>
ListNodePosi<T> ListNode<T>::insertAsPred(const T& e)
{
    ListNodePosi<T> x = new ListNode( e, pred, this ); // 新节点：前驱是当前节点的前驱，后继是当前节点
    pred->succ = x; // 原来的前驱节点的后继设为新的x节点
    pred = x; // 当前节点的前驱节点设为x节点
    return x;
}

template <typename T>
ListNodePosi<T> ListNode<T>::insertAsSucc(const T& e)
{
    ListNodePosi<T> x = new ListNode( e, this, succ ); // 新节点：前驱是当前节点，后继是当前节点的后继
    succ->pred = x; // 当前节点的后继节点的前驱设为新的x节点
    succ = x; // 当前节点的后继节点设为x节点
    return x;
}
