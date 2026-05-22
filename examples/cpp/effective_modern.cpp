template<typename T>
typename remove_reference<T>::type&& move(T&& t)
{
    using ReturnType = typename remove_reference<T>::type&&;
    return static_cast<ReturnType>(t);
}
