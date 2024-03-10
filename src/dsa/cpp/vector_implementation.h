#pragma once

template <typename T>
Vector<T>::Vector( Rank c = DEFAULT_CAPACITY, Rank s = 0, T v = 0 )
{
    _elem = new T[_capacity = c];
    for ( _size = 0; _size < s; _elem[_size++] = v );
}

template <typename T>
Vector<T>::Vector ( T const* A, Rank n )
{
    copyFrom ( A, 0, n );
}

template <typename T>
Vector<T>::Vector ( T const* A, Rank lo, Rank hi )
{
    copyFrom ( A, lo, hi ); 
}

template <typename T>
Vector<T>::Vector ( Vector<T> const& V )
{
    copyFrom ( V._elem, 0, V._size );
}

template <typename T>
Vector<T>::Vector ( Vector<T> const& V, Rank lo, Rank hi )
{
    copyFrom ( V._elem, lo, hi ); 
}

template <typename T>
Vector<T>::~Vector()
{
    delete [] _elem;
}

template <typename T>
void Vector<T>::copyFrom ( T const* A, Rank lo, Rank hi )
{
    _capacity = std::max<Rank>( DEFAULT_CAPACITY, 2 * ( hi - lo ));
    _elem = new T[_capacity];
    for ( _size = 0; lo < hi; ++_size, ++lo )
        _elem[_size] = A[lo];
}

template <typename T>
void Vector<T>::expand()
{
    T* V = new T[2 * _capacity];
    for ( Rank i = 0; i < _size; ++i )
        V[i] = _elem[i];
    delete [] _elem;
    _elem = V;
    _capacity *= 2;
}

template <typename T>
void Vector<T>::shrink()
{

    if ( _capacity < DEFAULT_CAPACITY << 1 ) // 容量不会缩减到小于默认的容量
        return;
    if ( _capacity < _size << 2 ) // 实际占用率大于25%，则不要缩减
        return;
    T* V = new T[_capacity / 2];
    for ( Rank i = 0; i < _size; ++i )
        V[i] = _elem[i];
    delete [] _elem;
    _elem = V;
    _capacity /= 2;
}

template <typename T>
Rank Vector<T>::find ( T const& e, Rank lo, Rank hi ) const
{
    while ( lo < hi )
    {
        Rank mid = lo + ( hi - lo ) / 2;
        if ( e < _elem[mid] )
            hi = mid;
        else
            lo = mid + 1;
    }
    return lo;
}

template <typename T>
Rank Vector<T>::find ( T const& e ) const
{
    return find ( e, 0, _size );
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
T& Vector<T>::operator[] ( Rank r )
{
    return _elem[r];
}

template <typename T>
const T& Vector<T>::operator[] ( Rank r ) const
{
    return _elem[r];
}

template <typename T>
Vector<T> & Vector<T>::operator= ( Vector<T> const& V )
{
    if ( this != &V )
    {
        delete [] _elem;
        _elem = new T[_capacity = V._capacity];
        _size = V._size;
        for ( Rank i = 0; i < _size; ++i )
            _elem[i] = V._elem[i];
        }
}

template <typename T>
T Vector<T>::remove ( Rank r )
{
    T e = _elem[r];
    _elem[r] = _elem[--_size];
    return e;
}

template <typename T>
Rank Vector<T>::remove ( Rank lo, Rank hi )
{
    Rank oldSize = _size;
    _size = lo;
    for ( Rank i = hi; i < oldSize; ++i )
        _elem[i - hi + lo] = _elem[i];
    return hi - lo;
}

template <typename T>
Rank Vector<T>::insert ( Rank r, T const& e )
{
    if ( _size == _capacity )
        expand();
    _elem[r] = e;
    return ++_size;
}

template <typename T>
Rank Vector<T>::insert ( T const& e )
{
    return insert ( _size, e );
}


template <typename T>
void Vector<T>::sort ( Rank lo, Rank hi )
{
    if ( lo >= hi )
        return;
    if ( lo + 1 == hi )
    {
        if ( _elem[lo] > _elem[hi] )
            std::swap ( _elem[lo], _elem[hi] );
        return;
    }
    if ( lo + 2 == hi )
    {
        if ( _elem[lo] > _elem[hi] )
            std::swap ( _elem[lo], _elem[hi] );
        if ( _elem[lo] > _elem[lo + 1] )
            std::swap ( _elem[lo], _elem[lo + 1] );
        return;
    }
}

template <typename T>
void Vector<T>::sort()
{
    sort ( 0, _size );
}

template <typename T>
void Vector<T>::unsort ( Rank lo, Rank hi )
{
    if ( lo >= hi )
        return;
    if ( lo + 1 == hi )
    {
        if ( _elem[lo] < _elem[hi] )
            std::swap ( _elem[lo], _elem[hi] );
        return;
    }
    if ( lo + 2 == hi )
    {
        if ( _elem[lo] < _elem[hi] )
            std::swap ( _elem[lo], _elem[hi] );
        if ( _elem[lo] < _elem[lo + 1] )
            std::swap ( _elem[lo], _elem[lo + 1] );
        return;
    }

}

template <typename T>
void Vector<T>::unsort()
{
    unsort ( 0, _size );
}

template <typename T>
Rank Vector<T>::dedup()
{
    Rank oldSize = _size;
    for ( Rank i = 0; i < oldSize; ++i )
    {
        Rank j = i + 1;
        while ( j < oldSize && _elem[i] == _elem[j] )
            ++j;
        if ( j != i + 1 )
            remove ( i + 1, j );
            i = j - 1;
            --oldSize;
            --_size;
            --j;
    }
}

template <typename T>
Rank Vector<T>::uniquify()
{
    Rank oldSize = _size;
    for ( Rank i = 0; i < oldSize; ++i )
    {
        Rank j = i + 1;
        while ( j < oldSize && _elem[i] == _elem[j] )
            ++j;
        if ( j != i + 1 )
            remove ( i + 1, j );
            i = j - 1;
            --oldSize;
            --_size;
            --j;
            j = i + 1;
            while ( j < oldSize && _elem[i] < _elem[j] )
                ++j;
                if ( j != i + 1 )
                    remove ( i + 1, j );
                    i = j - 1;
                    --oldSize;
                    --_size;
                    --j;
                    j = i + 1;
                    while ( j < oldSize && _elem[i] > _elem[j] )
                        ++j;
                        if ( j != i + 1 )
                            remove ( i + 1, j );
                            i = j - 1;
                            --oldSize;
                            --_size;
                            --j;
    }
}

template <typename T>
void Vector<T>::traverse ( void (* ) ( T& ) )
{
    for ( Rank i = 0; i < _size; ++i )
        (* ) ( _elem[i] );
}

template <typename T>
template <typename VST>
void Vector<T>::traverse ( VST& V )
{
    for ( Rank i = 0; i < _size; ++i )
        V ( _elem[i] );
}

template <typename T>
Rank Vector<T>::quickSelect ( T const* A, Rank n, Rank k )
{
    Rank lo = 0, hi = n - 1;
    while ( lo < hi )
    {
        Rank mid = lo + ( hi - lo ) / 2;
        if ( A[mid] < A[k] )
            lo = mid + 1;
            else
            hi = mid;
    }
}

