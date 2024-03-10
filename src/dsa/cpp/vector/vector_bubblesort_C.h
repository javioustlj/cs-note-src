
#pragma once

template <typename T> //向量的起泡排序（跳跃版）
void Vector<T>::bubbleSort( Rank lo, Rank hi ) { //assert: 0 <= lo < hi <= size
   for( Rank last; lo < hi; hi = last )
      for( Rank i = ( last = lo ) + 1; i < hi; i++ )
         if ( _elem[i - 1] > _elem[i] ) //若逆序，则
            swap( _elem[i - 1], _elem[i] ), last = i; //交换——记住最新（后）的交换位置
}
