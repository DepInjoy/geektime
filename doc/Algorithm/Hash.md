`std::unordered_map`是C++标准库提供的Hash存储的数据结构，关于其底层数据结构在Stack Overflow中有如下的描述：

> Every unordered_map implementation stores a linked list to external nodes in the array of buckets. Meaning that inserting an item will always allocate at least once (the new node) if not twice (resizing the array of buckets, then the new node).
>
> No, that is not at all the most efficient way to implement a hash map for most common uses. Unfortunately, a small "oversight" in the specification of unordered_map all but requires this behavior. The required behavior is that iterators to elements must stay valid when inserting or deleting other elements. Because inserting might cause the bucket array to grow (reallocate), it is not generally possible to have an iterator pointing directly into the bucket array and meet the stability guarantees.
>
> `unordered_map` is a better data structure if you are storing expensive-to-copy items as your key or value. Which makes sense, given that its general design was lifted from Boost's pre-move-semantics design[1].



# 参考资料

1. [stackOverFlow:c++ unordered_map collision handling , resize and rehash](https://stackoverflow.com/questions/31098123/c-unordered-map-collision-handling-resize-and-rehash)

