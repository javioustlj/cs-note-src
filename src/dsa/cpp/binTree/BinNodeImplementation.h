#pragma once

#include <deque>
#include <stack>
#include <iostream>

template <typename T>
BinNode<T>::BinNode()
    : parent( nullptr )
    , lc( nullptr )
    , rc( nullptr )
    , data( T() )
    , height( 0 )
{
}

template <typename T>
BinNode<T>::BinNode(const T& e,
                    BinNodePosi<T> p,
                    BinNodePosi<T> lc,
                    BinNodePosi<T> rc,
                    int h)
        : data( e )
        , parent( p )
        , lc( lc )
        , rc( rc )
        , height( h )
{
}

template <typename T>
Rank BinNode<T>::size() const
{
    Rank s = 1; //计入本身
    if ( lc ) s += lc->size(); //递归计入左子树规模
    if ( rc ) s += rc->size(); //递归计入右子树规模
    return s;
}

template <typename T>
Rank BinNode<T>::updateHeight()
{
    int child_height = stature( lc ) > stature( rc ) ? stature( lc ) : stature( rc );
    return height = 1 + child_height;
}

template <typename T>
void BinNode<T>::updateHeightAbove()
{ 
    for ( BinNodePosi<T> x = this; x; x = x->parent )
        x->updateHeight(); 
} //可优化

template <typename T>
BinNodePosi<T> BinNode<T>::insertAsLC( T const& e )
{
    return lc = new BinNode( e, this );
}

template <typename T>
BinNodePosi<T> BinNode<T>::insertAsRC( T const& e )
{
    return rc = new BinNode( e, this ); 
}

template <typename T> template <typename VST>
void BinNode<T>::travLevel( VST& visit)
{

    std::deque<BinNodePosi<T>> q;
    BinNodePosi<T> p = this;
    q.push_back(p);
    for ( ; !q.empty(); )
    {
        BinNodePosi<T> x = q.front(); 
        q.pop_front();
        // std::cout << x->data << " ";
        visit(x->data);
        if (x->hasLChild()) 
            q.push_back(x->lc);
        if (x->hasRChild()) 
            q.push_back(x->rc);
    }
}

template <typename T> template <typename VST>
void BinNode<T>::travPreOrder( VST& visit)
{
    std::stack<BinNodePosi<T>> s;
    for ( BinNodePosi<T> p = this; p || !s.empty(); )
    {
        for ( ; p; p = p->lc )
        {
            visit(p->data);
            s.push(p);
        }

        if (!s.empty())
        {
            p = s.top();
            s.pop();
            p = p->rc;
        }
    }
}

template <typename T> template <typename VST>
void BinNode<T>::travInOrder( VST& visit)
{
    std::stack<BinNodePosi<T>> s;
    for ( BinNodePosi<T> p = this; p || !s.empty(); )
    {
        for ( ; p; p = p->lc )
            s.push(p);
        if (!s.empty())
        {
            p = s.top();
            s.pop();
            visit(p->data);
            p = p->rc;
        }
    }
}

template <typename T> template <typename VST>
void BinNode<T>::travPostOrder( VST& visit)
{
    std::stack<BinNodePosi<T>> s;
    BinNodePosi<T> temp;
    for ( BinNodePosi<T> p = this; p || !s.empty(); )
    {
        for ( ; p; p = p->lc )
        {
            s.push(p);
        }

        if (!s.empty())
        {
            p = s.top();
            if (p->hasRChild() && p->rc != temp)
            {
                p = p->rc;
            }
            else
            {
                visit(p->data);
                temp = p;
                s.pop();
                p = nullptr;
            }
        }
    }
}

template <typename T>
bool BinNode<T>::hasLChild() const
{
    return lc;
}

template <typename T>
bool BinNode<T>::hasRChild() const
{
    return rc;
}

template <typename T>
bool BinNode<T>::hasChild() const
{
    return lc || rc;
}

template <typename T>
bool BinNode<T>::hasBothChild() const
{
    return lc && rc;
}

template <typename T>
bool BinNode<T>::hasParent() const
{
    return parent;
}

template <typename T>
bool BinNode<T>::isLeaf() const
{
    return !hasChild();
}

template <typename T>
bool BinNode<T>::isRoot() const
{
    return !hasParent();
}

template <typename T>
bool BinNode<T>::isLChild() const
{
    return parent && parent->lc == this;
}

template <typename T>
bool BinNode<T>::isRChild() const
{
    return parent && parent->rc == this;
}

template <typename T> 
BinNodePosi<T>&  BinNode<T>::fromParentTo( )
{
    if ( isRoot() )
        return parent;
    if ( isLChild() )
        return parent->lc;
    if ( isRChild() )
        return parent->rc;
    // return nullptr;
}
