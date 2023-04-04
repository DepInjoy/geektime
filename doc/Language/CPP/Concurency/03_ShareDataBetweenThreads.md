# 死锁

如果有两个线程，都需要同时锁住两个互斥，才可以进行某项操作，但它们分别都只锁住了一个互斥，都等着再给另一个互斥加锁。因为它们同在等待对方解锁互斥，这种情形称为**死锁(deadlock)**。防范死锁的建议通常是，始终按相同顺序对两个互斥加锁，若我们总是先锁互斥A再锁互斥B，则永远不会发生死锁。但是，这样也会存在特殊情况而陷入死锁：

> 例如，运用多个互斥分别保护多个独立的实例，这些实例属于同一个类。考虑一个函数，其操作同一个类的两个实例，互相交换它们的内部数据。为了保证互换正确完成，免受并发改动的不良影响，两个实例上的互斥都必须加锁。如果选用了固定的次序（两个对象通过参数传入，总是先给第一个实例的互斥加锁，再轮到第二个实例的互斥），前面的建议就适得其反：针对两个相同的实例，若两个线程都通过该函数在它们之间互换数据，只是两次调用的参数顺序相反，会导致它们陷入死锁！

```C++
// 先对lhs加锁,再对rhs加锁
swap(lhs, rhs);  // 1
// 先对rhs加锁, 再对lhs加锁
swap(rhs, lhs); // 2
// 这样存在1持有lhs的锁等待rhs的锁，而2持有rhs的锁等待lhsde锁情况
// 进而导致死锁
```

## 同时获取锁,防死锁处理

C++标准库提供了`std::lock()`函数，专门解决这一问题，它可以同时锁住多个互斥，而没有发生死锁的风险。

```C++
template< class Lockable1, class Lockable2, class... LockableN >
void lock(Lockable1& lock1, Lockable2& lock2, LockableN&... lockn);
```

`std::lock`采用死锁避免算法lock给定的`Lockable`对象`lock1, lock2, ..., lockn`防止死锁。它以一种不确定的调用顺序来调用`lock, try_lock, unlock`，如果调用`lock`或`unlock`导致了异常，所有锁住的对象在将异常再次抛出之前，都会调用`unlock`。

下面是运用`std::lock()`函数和`std::lock_guard<>`类模板，进行内部数据的互换操作。

```C++
class some_big_object;
void swap(some_big_object& lhs, some_big_object& rhs);

class X {
private:
    some_big_object some_detail;
    std::mutex m;
public:
    X(some_big_object const& sd):some_detail(sd){}
    friend void swap(X& lhs, X& rhs) {
        // 这是必须的，如果lhs和rhs指向同一个对象
        // 在同一个mutex对象上获取了锁，再次试图获取锁会导致未定义行为
        if (&lhs == &rhs) {
            return;
        }
 
        // 采用std::lock锁住两个互斥
        std::lock(lhs.m, rhs.m);
        // 构造std::lock_guard并提供std::adopt_lock对象表明互斥已被锁住
        // 也就是说互斥上已有锁，std::lock_guard实例接受锁的归属权，不得在构造函数内尝试另行加锁
        // 无论函数正常退出或抛出异常而退出, std::lock_guard保证互斥全部解锁
        std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock);
        std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock);
        swap(lhs.some_detail, rhs.some_detail);
    }
};
```

`std::lock()`在其内部对`lhs.m`或`rhs.m`加锁，可能导致抛出异常，异常便会从`std::lock()`向外传播。假如`std::lock()`函数在其中一个互斥上成功获取了锁，但它试图在另一个互斥上获取锁时却有异常抛出，那么第一个锁就会自动释放：若加锁操作涉及多个互斥，则`std::lock()`函数的语义是"全员共同成败"(all-or-nothing，或全部成功锁定，或没获取任何锁并抛出异常)。

针对上述场景，C++17进一步提供了RAII类模板`std::scoped_lock<>`。`std::scoped_lock<>`和`std::lock_guard<>`完全等价，前者是可变参数模板(`variadic template`)，接收各种互斥型别作为模板参数列表，可以多个互斥对象作为构造函数的参数列表。上面`std::lock`的代码可以等价改写成下面的实现：

```C++
void swap(X& lhs, X& rhs) {
	if(&lhs == &rhs) {
        return;
    }

    // 传入的两个互斥都被加锁，机制与std::lock相同
    // C++17支持类模板参数推导，有隐式类模板参数推导
    // (implicit class template parameter deduction)机制
    // 等价于 std::scoped_lock<std::mutex,std::mutex> guard(lhs.m,rhs.m);
    std::scoped_lock guard(lhs.m,rhs.m);
    swap(lhs.some_detail,rhs.some_detail);
} // 析构完成，两个互斥都被解锁
```

假定我们需要同时获取多个锁，那么`std::lock()`函数和`std::scoped_lock<>`模板可帮助防范死锁，但若代码分别获取各个锁需要程序员来防范死锁的风险。