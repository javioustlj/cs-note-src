#pragma once

template <typename T>
List<T>::List()
{
    init();
}

template <typename T>
List<T>::List(const List<T>& L )
{
    init();
    copyNodes( L.first(), L.size() );
}

template <typename T>
List<T>::List( List<T> const& L, Rank r, Rank n )
{
    init();
    ListNodePosi<T> p = L.first();
    for ( ; n--; )
        p = p->succ;
    copyNodes ( p, n );
}

template <typename T>
List<T>::List( ListNodePosi<T> p, Rank n)
{
    init();
    copyNodes( p, n );
}

template <typename T>
List<T>::~List()
{
    clear();
    delete header;
    delete trailer;
}

template <typename T>
Rank List<T>::size() const
{
    return _size;
}

template <typename T>
bool List<T>::empty() const
{
    return !_size;
}

template <typename T>
ListNodePosi<T> List<T>::operator[] (Rank r) const
{
    assert( r >= 0 && r < _size );
    ListNodePosi<T> p = first();
    while ( r-- )
        p = p->succ;
    return p;
}

template <typename T>
ListNodePosi<T> List<T>::first() const
{
    return header->succ;
}

template <typename T>
ListNodePosi<T> List<T>::last() const
{
    return trailer->pred;
}

template <typename T>
bool List<T>::valid( ListNodePosi<T> p ) const
{
    return p && ( trailer != p ) && ( header != p );
}

template <typename T>
ListNodePosi<T> List<T>::find( T const& e ) const
{
    return find( e, _size, trailer );
}

template <typename T>
ListNodePosi<T> List<T>::find( T const& e, Rank n, ListNodePosi<T> p ) const
{
    for ( ; n && p != header; --n, p = p->pred )
    {
        if (p->data == e)
            return p;
    }

    return nullptr;
}

template <typename T>
ListNodePosi<T> List<T>::insertAsFirst( const T &e )
{
    _size++;
    return header->insertAsSucc( e );
}

template <typename T>
ListNodePosi<T> List<T>::insertAsLast( T const& e )
{ 
    _size++; 
    return trailer->insertAsPred( e ); 
}

template <typename T> ListNodePosi<T>
List<T>::insert( ListNodePosi<T> p, T const& e )
{
    _size++;
    return p->insertAsSucc( e );
}

template <typename T> 
ListNodePosi<T> List<T>::insert( T const& e, ListNodePosi<T> p )
{ 
    _size++;
    return p->insertAsPred( e );
}

template <typename T>
T List<T>::remove( ListNodePosi<T> p )
{
    assert( header != p && trailer != p && p);
    T e = p->data;
    p->pred->succ = p->succ; // 前驱的后继指向p的后继
    p->succ->pred = p->pred; // 后继的前驱指向p的前驱
    _size--;
    delete p;
    return e;
}

template <typename T>
void List<T>::traverse( void ( *visit )( T& ) )
{
    for ( ListNodePosi<T> p = header->succ; p != trailer; p = p->succ )
        visit ( p->data );
}

template <typename T> template <typename VST>
void List<T>::traverse( VST& visit )
{  
    for ( ListNodePosi<T> p = header->succ; p != trailer; p = p->succ )
    { 
        visit ( p->data );  
    }
}

template <typename T>
void List<T>::init()
{
    header = new ListNode<T>;
    trailer = new ListNode<T>;
    header->succ = trailer;
    trailer->pred = header;
    _size = 0;
}

template <typename T>
Rank List<T>::clear()
{
    Rank oldSize = _size;
    for ( ; _size; )
        remove ( header->succ );

    return oldSize;
}

template <typename T>
void List<T>::copyNodes( ListNodePosi<T> p, Rank n )
{
    for ( ; n--; )
    {
        insertAsLast( p->data );
        p = p->succ; 
    }
}
