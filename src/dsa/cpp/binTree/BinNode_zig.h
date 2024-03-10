
#pragma once

template <typename T> BinNodePosi<T> BinNode<T>::zig() { //顺时针旋转
   BinNodePosi<T> v = lc; if ( !v ) return this;
   v->parent = parent;
   if ( v->parent )
      ( ( this == v->parent->rc ) ? v->parent->rc : v->parent->lc ) = v;
   lc = v->rc; if ( lc ) lc->parent = this;
   v->rc = this; parent = v;
   updateHeight(); v->updateHeight();
   return v;
}
