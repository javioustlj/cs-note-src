
#pragma once

template <typename T> BinNodePosi<T> BinTree<T>::insert( T const& e )
   { _size = 1; return _root = new BinNode<T>( e ); } //将e当作根节点插入空的二叉树

template <typename T> BinNodePosi<T> BinTree<T>::insert( T const& e, BinNodePosi<T> x )
   { _size++; x->insertAsLC( e ); x->updateHeightAbove(); return x->lc; } // e插入为x的左孩子

template <typename T> BinNodePosi<T> BinTree<T>::insert( BinNodePosi<T> x, T const& e )
   { _size++; x->insertAsRC( e ); x->updateHeightAbove(); return x->rc; } // e插入为x的右孩子
