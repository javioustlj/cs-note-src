
#include "queen_stack.h"

Rank N = 0; //棋盘大小

void displayRow ( Queen& q ) { //打印当前皇后（放置于col列）所在行
   printf ( "%2d: ", q.x );
   Rank i = 0;
   while ( i++ < q.y ) printf ( "[]" );
   printf ( "█" );
   while ( i++ < N ) printf ( "[]" );
   printf ( "%2d\n", q.y );
}

void displayProgress ( Stack<Queen>& S, Rank nQueen ) { //在棋盘上显示搜查的进展
   system ( "cls" );
   N = nQueen; S.traverse ( displayRow );
   if ( nQueen <= S.size() ) {
      cout  << nSolu << " solution(s) found after " << nCheck << " check(s)\a";
      getchar();
   }
}
