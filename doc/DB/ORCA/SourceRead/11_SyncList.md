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



# SLink

```C++
struct SLink {
	// link forward/backward
	void *m_next{nullptr};
	void *m_prev{nullptr};
};
```

通过`SLink`结构在向单链表结构中插入和删除元素时操作的都是由前向节点和后向节点组成的`Slink`，可以实现以`O(1)`的时间复杂度访问任意节点的前向节点和后向节点。

# CList

单向链表，借助`SLink`实现元素的插入、删除，可以达到以`O(1)`的时间复杂度访问任意节点的前向节点和后向节点

```C++
template <class T>
class CList {
	friend class CSyncList<T>;
private:
	// T中拥有一个T类型的指针,m_offset代表该成员的偏移量
	ULONG m_offset{gpos::ulong_max};
    // 有效元素T的数量
	ULONG m_size{0};
	
    // 拥有头指针和尾指针可以实现在头部前和尾部后添加元素
	T *m_head;
	T *m_tail;
};
```

提供 对外的操作

```
初始化，其中offset表示T类型指针成员的偏移量,将该值存储于m_offset
void Init(ULONG offset);

void Prepend(T* elem);

void Append(T *elem);

void Remove(T *elem);

T *RemoveHead()

T* RemoveTail();

T* First() const;
T* Last() const;
T *Next(const T* pt) const;
T* Prev(const T* pt) const;
ULONG Size() const;
BOOL IsEmpty() const;
```



```C++
void Prepend(T *elem, T *next_elem) {
    T *prev = static_cast<T *>(Link(next_elem).m_prev);

    SLink &link = Link(elem);
    link.m_next = next_elem;
    link.m_prev = prev;

    Link(next_elem).m_prev = elem;
    if (nullptr != prev) {
        // inserted not at head, ie valid prev element
        Link(prev).m_next = elem;
    } else {
        // prepended to head
        m_head = elem;
    }

    m_size++;
}
```

