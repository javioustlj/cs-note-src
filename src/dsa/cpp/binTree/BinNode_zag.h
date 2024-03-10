
#pragma once

template <typename T> BinNodePosi<T> BinNode<T>::zag() { //逆时针旋转
   BinNodePosi<T> v = rc; if ( !v ) return this;
   v->parent = parent;
   if ( v->parent )
      ( ( this == v->parent->lc ) ? v->parent->lc : v->parent->rc ) = v;
   rc = v->lc; if ( rc ) rc->parent = this;
   v->lc = this; parent = v;
   updateHeight(); v->updateHeight();
   return v;
}
