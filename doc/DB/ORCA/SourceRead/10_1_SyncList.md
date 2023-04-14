# CSyncList

`CSyncList`要求T拥有一个指向T类型的指针，`void Init(ULONG offset)`中的`offset`代表的是T类型指针成员的偏移量，它借助`CList`实现可以再头部插入元素，获取头部和下一个元素。相关的接口和一些定义如下：

```C++
template <class T>
class CSyncList{
private:
	// underlying list
	CList<T> m_list;
};
```

对外提供的操作:

```
offset: T类型指针成员的偏移量
void Init(ULONG offset)

在list的头部插入elem
void Push(T *elem)

将list头部元素删除并返回,如果list空则返回null
T *Pop()

获取list头部元素
T * PtFirst() const

获取elem的下一个元素
T * Next(T *elem) const

判断list是否为空
BOOL IsEmpty() const
```

# CList



```C++
template <class T>
class CList {
	friend class CSyncList<T>;
private:
	// T中拥有一个T类型的指针,m_offset代表该成员的偏移量
	ULONG m_offset{gpos::ulong_max};
	ULONG m_size{0};
	
    // 拥有头指针和尾指针可以实现在头部前和尾部后添加元素
	T *m_head;
	T *m_tail;
};
```

