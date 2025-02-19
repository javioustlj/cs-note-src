


- `__device__` 表示从GPU调用且运行在GPU上面
- `__forceinline__` 表示 内联函数


```cpp
// alignas(16) 表示16字节对齐
struct alignas(16) ncclDevWorkColl
{

};
```
