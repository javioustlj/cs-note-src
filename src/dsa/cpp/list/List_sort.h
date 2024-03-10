
#pragma once

template <typename T> void List<T>::sort ( ListNodePosi<T> p, Rank n ) { //列表区间排序
   /*DSA*/// switch ( 1 ) {
   switch ( rand() % 4 ) { //随机选取排序算法。可根据具体问题的特点灵活选取或扩充
      case 1  : insertionSort( p, n ); break; //插入排序
      case 2  : selectionSort( p, n ); break; //选择排序
      case 3  :     mergeSort( p, n ); break; //归并排序
      default :     radixSort( p, n ); break; //基数排序
   }
}
