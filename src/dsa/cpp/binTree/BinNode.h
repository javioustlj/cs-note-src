#pragma once

#define stature( p ) ( (int)( ( p ) ? ( p )->height : -1 ) ) //外部节点高度为-1，以上递推

using Rank = unsigned int;

template <typename T> struct BinNode;
template <typename T> using BinNodePosi = BinNode<T>*; //节点位置

template <typename T> struct BinNode
{
    // 构造函数
    BinNode();
    BinNode( const T& e,
             BinNodePosi<T> p = nullptr,
             BinNodePosi<T> lc = nullptr,
             BinNodePosi<T> rc = nullptr,
             int h = 0
            );
    // 操作接口
    Rank size() const; //统计当前节点后代总数，亦即以其为根的子树的规模
    Rank updateHeight(); //更新当前节点高度
    void updateHeightAbove(); //更新当前节点及其祖先的高度
    BinNodePosi<T> insertAsLC( T const& ); //作为当前节点的左孩子插入新节点
    BinNodePosi<T> insertAsRC( T const& ); //作为当前节点的右孩子插入新节点
    BinNodePosi<T> succ(); //取当前节点的直接后继
    template <typename VST> void travLevel( VST& ); //子树层次遍历
    template <typename VST> void travPreOrder( VST& ); //子树先序遍历
    template <typename VST> void travInOrder( VST& ); //子树中序遍历
    template <typename VST> void travPostOrder( VST& ); //子树后序遍历

    // 比较器、判等器（各列其一，其余自行补充）
    bool operator< ( BinNode const& bn ) { return data < bn.data; } //小于
    bool operator== ( BinNode const& bn ) { return data == bn.data; } //等于
    // /*DSA*/
    // /*DSA*/BinNodePosi<T> zig(); //顺时针旋转
    // /*DSA*/BinNodePosi<T> zag(); //逆时针旋转
    // /*DSA*/BinNodePosi<T> balance(); //完全平衡化
    // /*DSA*/BinNodePosi<T> imitate( const BinNodePosi<T> ); //临摹

    bool hasLChild() const;
    bool hasRChild() const;
    bool hasChild() const;
    bool hasBothChild() const;
    bool hasParent() const;
    bool isLChild() const;
    bool isRChild() const;
    bool isRoot() const;
    bool isLeaf() const;
    BinNodePosi<T>& fromParentTo();

    BinNodePosi<T> parent; // 父亲
    BinNodePosi<T> lc;  // 左子
    BinNodePosi<T> rc;  // 右子
    Rank height;        // 高度
    T data;  //数据元素
};

#include "BinNodeImplementation.h"
