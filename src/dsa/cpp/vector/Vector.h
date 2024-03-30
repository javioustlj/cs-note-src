#pragma once

using Rank = unsigned int;

template<typename T> class Vector
{
public:
    // 构造函数
	Vector(Rank _capacity = DEFAULT_CAPACITY);
    Vector ( const T *A, Rank n ); //从整个数组构造
    Vector ( const T *A, Rank lo, Rank hi ); // 从数组区间构造
    Vector ( const Vector<T>& V ); //拷贝构造
    Vector& operator= ( const Vector<T>& V ); //拷贝赋值构造
    Vector ( const Vector<T>& V, Rank lo, Rank hi ); //向量区间构造
    Vector ( Vector<T>&& V ); // 移动构造
    Vector& operator= ( Vector<T>&& V ); // 移动赋值构造

    // 析构函数
    ~Vector();

    // 只读成员函数
    Rank size() const;
    bool empty() const;
    Rank find ( T const& e ) const; //无序向量整体查找
    Rank find ( T const& e, Rank lo, Rank hi ) const; //无序向量区间查找

    // 可写成员函数
    T& operator[] ( Rank r ); //重载下标操作符，可以类似于数组形式引用各元素
    const T& operator[] ( Rank r ) const; //仅限于做右值的重载版本
    Rank insert ( Rank r, T const& e ); //插入元素
    Rank insert ( T const& e ); //默认作为末元素插入
    Rank remove ( Rank r ); //删除秩为r的元素
    Rank remove ( Rank lo, Rank hi ); //删除秩在区间[lo, hi)之内的元素

    void traverse ( void (* ) ( T& ) ); //遍历（使用函数指针，只读或局部性修改）
    template <typename VST> void traverse ( VST& ); //遍历（使用函数对象，可全局性修改）
protected:
    Rank _size;
    Rank _capacity;
    T* _elem;
    static constexpr Rank DEFAULT_CAPACITY = 8;
    static constexpr Rank NOT_FOUND = -1;
    void copyFrom ( const T *A, Rank lo, Rank hi ); //复制数组区间A[lo, hi)
    void expand(); //空间不足时扩容
    void shrink(); // 释放空间
};

#include "VectorImpemention.h"
