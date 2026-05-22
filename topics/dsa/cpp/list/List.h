#pragma once

#include "ListNode.h"

template <typename T> class List
{
public:
    // 构造函数
    List(); //默认
    List( List<T> const& L ); // 赋值构造
    List( List<T> const& L, Rank r, Rank n ); //复制列表L中自第r项起的n项
    List( ListNodePosi<T> p, Rank n ); //复制列表中自位置p起的n项
    // 析构函数
    ~List(); // 析构：释放（包含头、尾哨兵在内的）所有节点
    // 只读访问接口
    Rank size() const; //规模
    bool empty() const; //判空
    ListNodePosi<T> operator[]( Rank r ) const; //重载，支持循秩访问O(n)
    ListNodePosi<T> first() const;
    ListNodePosi<T> last() const;
    bool valid( ListNodePosi<T> p ) const;
    ListNodePosi<T> find( T const& e ) const; //无序列表查找
    ListNodePosi<T> find( T const& e, Rank n, ListNodePosi<T> p ) const; //无序区间查找

    // 可写访问接口
    ListNodePosi<T> insertAsFirst( T const& e ); //将e当作首节点插入
    ListNodePosi<T> insertAsLast( T const& e ); //将e当作末节点插入
    ListNodePosi<T> insert( ListNodePosi<T> p, T const& e ); //将e当作p的后继插入
    ListNodePosi<T> insert( T const& e, ListNodePosi<T> p ); //将e当作p的前驱插入
    T remove( ListNodePosi<T> p ); //删除合法位置p处的节点,返回被删除节点
    // 遍历
    void traverse( void ( * )( T& ) ); //依次实施visit操作（函数指针）
    template <typename VST> void traverse( VST& ); //依次实施visit操作（函数对象）

protected:
    void init(); //列表创建时的初始化
    Rank clear(); //清除所有节点
    void copyNodes( ListNodePosi<T>, Rank ); //复制列表中自位置p起的n项

private:
    Rank _size;              // 规模
    ListNodePosi<T> header;  // 头哨兵
    ListNodePosi<T> trailer; // 尾哨兵
};

#include "ListImplementation.h"
