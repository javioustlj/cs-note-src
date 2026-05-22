#pragma once

template <typename T> 
Vector<T>::Vector(Rank capacity)
    : _elem(new T[capacity])
    ,_size(0)
    ,_capacity(capacity)
{
}

template <typename T>
Vector<T>::Vector(const T *A, Rank n)
{
    copyFrom(A, 0, n);
}

template <typename T>
Vector<T>::Vector(const T *A, Rank lo, Rank hi)
{
    copyFrom(A, lo, hi);
}

template <typename T>
Vector<T>::Vector(const Vector<T> &v)
{
    copyFrom(v._elem, 0, v._size);
}

template <typename T>
Vector<T>& Vector<T>::operator= ( const Vector<T> &v )
{
    if ( this != &v )
    {
        delete [] _elem;
        copyFrom(v._elem, 0, v._size);
    }
    return *this;
}

template <typename T>
Vector<T>::Vector(const Vector<T> &v, Rank lo, Rank hi)
{
    copyFrom(v._elem, lo, hi);
}

template <typename T>
Vector<T>::Vector(Vector<T> &&v)
{
    _elem = v._elem;
    _size = v._size;
    _capacity = v._capacity;

    v._elem = nullptr;
    v._size = v._capacity = 0;
}

template <typename T>
Vector<T>& Vector<T>::operator= ( Vector<T> &&v )
{
    if ( this != &v )
    {
        delete [] _elem;
        _elem = v._elem;
        _size = v._size;
        _capacity = v._capacity;

        v._elem = nullptr;
        v._size = v._capacity = 0;
    }
    return *this;
}

template <typename T>
Vector<T>::~Vector()
{
    delete [] _elem;
}

template <typename T>
Rank Vector<T>::size() const
{
    return _size;
}

template <typename T>
bool Vector<T>::empty() const
{
    return !_size;
}

template <typename T>
Rank Vector<T>::find ( T const& e ) const
{
    return find ( e, 0, _size );
}

template <typename T>
Rank Vector<T>::find ( T const& e, Rank lo, Rank hi ) const
{
    for ( ; lo < hi; lo++ )
        if ( _elem[ lo ] == e )
            return lo;
    return NOT_FOUND;
}

template <typename T>
T& Vector<T>::operator[] ( Rank r )
{
    return _elem[ r ];
}

template <typename T>
const T& Vector<T>::operator[] ( Rank r ) const
{
    return _elem[ r ];
}

template <typename T>
Rank Vector<T>::insert ( Rank r, T const& e )
{
    if ( r > _size )
        return NOT_FOUND;
    expand();
    for ( Rank i = _size; r < i; i-- )
        _elem[ i ] = _elem[ i - 1 ];
    _elem[ r ] = e;
    _size++;
    return r;
}

template <typename T>
Rank Vector<T>::insert ( T const& e )
{
    return insert( _size, e );
}

template <typename T>
Rank Vector<T>::remove ( Rank r )
{
    return remove( r, r + 1 );
}

template <typename T>
Rank Vector<T>::remove (Rank lo, Rank hi)
{
    if ( lo > hi )
        return NOT_FOUND;
    if ( lo == hi)
        return 0;
    for ( ; hi < _size; )
        _elem[lo++] = _elem[hi++];
    _size = lo;
    shrink();
    return hi - lo;
}

template <typename T>
void Vector<T>::traverse( void ( *visit )( T& ) )
{
    for ( Rank i = 0; i < _size; i++ ) 
        visit( _elem[i] ); 
}

template <typename T> template <typename VST>
void Vector<T>::traverse( VST& visit )
{ 
    for ( Rank i = 0; i < _size; i++ ) 
        visit( _elem[i] );
}


template <typename T> //T为基本类型，或已重载赋值操作符'='
void Vector<T>::copyFrom (const T *A, Rank lo, Rank hi )
{
    _capacity = DEFAULT_CAPACITY;
    if ( _capacity < 2 * ( hi - lo) )
        _capacity = 2 * ( hi - lo);
   _elem = new T[ _capacity ];
   for ( _size = 0; lo < hi; _size++, lo++ )
      _elem[ _size ] = A[ lo ];
} 

template <typename T>
void Vector<T>::expand()
{
    if ( _size < _capacity )
        return;
    if ( _capacity < DEFAULT_CAPACITY )
        _capacity = DEFAULT_CAPACITY;
    T* oldElem = _elem;
    _elem = new T[_capacity <<= 1];
    for ( Rank i = 0; i < _size; ++i )
        _elem[i] = oldElem[i];
    delete [] oldElem;
}

template <typename T>
void Vector<T>::shrink()
{
    if ( _capacity < DEFAULT_CAPACITY << 1 ) 
        return;
    if ( _capacity < _size << 2 )
        return;
    T* oldElem = _elem;
    _elem = new T[_capacity >>= 1];
    for ( Rank i = 0; i < _size; i++ ) 
        _elem[i] = oldElem[i];
    delete[] oldElem;
}

