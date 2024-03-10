
#pragma once

template <typename T> //在有序向量的区间[lo, hi)内，确定不大于e的最后一个节点的秩
Rank Vector<T>::search( T const& e, Rank lo, Rank hi ) const { // 0 <= lo < hi <= _size
   return ( rand() % 2 ) ? binSearch( _elem, e, lo, hi ) : fibSearch( _elem, e, lo, hi );
} //等概率地随机使用二分查找、Fibonacci查找
