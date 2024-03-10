
#pragma once

template <typename T> Rank BinNode<T>::updateHeight() //更新当前节点高度
   { return height = 1 + max( stature( lc ), stature( rc ) ); } //具体规则，因树而异

template <typename T> void BinNode<T>::updateHeightAbove() //更新当前节点及其祖先的高度
   { for ( BinNodePosi<T> x = this; x; x = x->parent ) x->updateHeight(); } //可优化
