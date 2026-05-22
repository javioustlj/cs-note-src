#pragma once

template <typename T>
BinTree<T>::BinTree()
    : _size(0)
    , _root(nullptr)
{
}


template <typename T>
BinTree<T>::~BinTree()
{
    if ( 0 < _size ) 
        remove( _root );
}

template <typename T>
Rank BinTree<T>::size() const
{
    return _size;
}

template <typename T>
bool BinTree<T>::empty() const
{
    return !_size;
}

template <typename T>
BinNodePosi<T> BinTree<T>::root() const
{
    return _root;
}

//将e当作根节点插入空的二叉树
template <typename T>
BinNodePosi<T> BinTree<T>::insert( T const& e )
{ 
    _size = 1;
    return _root = new BinNode<T>( e ); 
}

// e插入为x的左孩子
template <typename T>
BinNodePosi<T> BinTree<T>::insert( T const& e, BinNodePosi<T> x )
{ 
    _size++;
    x->insertAsLC( e );
    x->updateHeightAbove();
    return x->lc;
}

// e插入为x的右孩子
template <typename T>
BinNodePosi<T> BinTree<T>::insert( BinNodePosi<T> x, T const& e )
{ 
    _size++;
    x->insertAsRC( e );
    x->updateHeightAbove();
    return x->rc; 
}

//将S当作节点x的左子树接入二叉树，S本身置空
// x->lc == nullptr
template <typename T> 
BinNodePosi<T> BinTree<T>::attach( BinTree<T>*& S, BinNodePosi<T> x )
{
    if ( x->lc = S->_root )
    x->lc->parent = x; 
    _size += S->_size;
    x->updateHeightAbove(); //更新全树规模与x所有祖先的高度
    S->_root = nullptr;
    S->_size = 0;
    //    release( S );
    S = nullptr;
    return x; //释放原树，返回接入位置
}

//将S当作节点x的右子树接入二叉树，S本身置空
 // x->rc == nullptr
template <typename T>
BinNodePosi<T> BinTree<T>::attach( BinNodePosi<T> x, BinTree<T>*& S ) {
    if ( x->rc = S->_root )
        x->rc->parent = x;
    _size += S->_size;
    x->updateHeightAbove(); //更新全树规模与x所有祖先的高度
    S->_root = nullptr; 
    S->_size = 0; 
    release( S ); 
    S = nullptr; 
    return x; //释放原树，返回接入位置
}

//删除二叉树中位置x处的节点及其后代，返回被删除节点的数值
template <typename T> 
Rank BinTree<T>::remove( BinNodePosi<T> x ) 
{ 
    // assert: x为二叉树中的合法位置
    x->fromParentTo() = nullptr; //切断来自父节点的指针
    x->parent->updateHeightAbove(); //更新祖先高度
    Rank n = removeAt( x ); 
    _size -= n; 
    return n;
}

template <typename T> //删除二叉树中位置x处的节点及其后代，返回被删除节点的数值
static Rank removeAt( BinNodePosi<T> x ) { // assert: x为二叉树中的合法位置
   if ( !x ) return 0; //递归基：空树
   Rank n = 1 + removeAt( x->lc ) + removeAt( x->rc ); //递归释放左、右子树
//    release( x->data ); 
//    release( x ); 
   delete x;
   return n; //释放被摘除节点，并返回删除节点总数
}


//二叉树子树分离算法：将子树x从当前树中摘除，将其封装为一棵独立子树返回
template <typename T> 
BinTree<T>* BinTree<T>::secede( BinNodePosi<T> x ) { // assert: x为二叉树中的合法位置
   x->FromParentTo() = nullptr; //切断来自父节点的指针
   x->parent->updateHeightAbove(); //更新原树中所有祖先的高度
   BinTree<T>* S = new BinTree<T>; 
   S->_root = x;   //新树以x为根
   x->parent = nullptr; 
   S->_size = x->size(); //更新规模
   _size -= S->_size; 
   return S;
}

template <typename T> template <typename VST>
void BinTree<T>::travLevelOrder( VST& visit )
{
    if ( _root )
        _root->travLevel( visit );
}

template <typename T> template <typename VST>
void BinTree<T>::travPreOrder( VST& visit )
{
    if ( _root )
        _root->travPreOrder( visit );
}

template <typename T> template <typename VST>
void BinTree<T>::travInOrder( VST& visit )
{
    if ( _root )
        _root->travInOrder( visit );
}

template <typename T> template <typename VST>
void BinTree<T>::travPostOrder( VST& visit )
{
    if ( _root )
        _root->travPostOrder( visit ); 
}
