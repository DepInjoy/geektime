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



C++标准库提供了`std::lock()`函数，专门解决同时获取锁可能导致的死锁问题，它可以同时锁住多个互斥，而没有发生死锁的风险。

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



# 层级锁

锁的层级划分就是按特定方式规定加锁次序，在运行期据查验加锁操作是否遵从预设规则。按照构思，我们把应用程序分层，并且明确每个互斥位于哪个层级。若某线程已对低层级互斥加锁，则不准它再对高层级互斥加锁。具体做法是将层级的编号赋予对应层级应用程序上的互斥，并记录各线程分别锁定了哪些互斥。C++标准目前尚未支持。

示例两个线程如何应用层级互斥，利用层级锁防范死锁

```C++
// 依据层级编号构建层级锁,层级越低编号越小
// 这套机制旨在设定加锁的规则，如果已在某hierarchical_mutex互斥上持有锁
// 那么只能由相对低层级的hierarchical_mutex互斥获取锁，从而限制代码行为
hierarchical_mutex high_level_mutex(10000);
hierarchical_mutex low_level_mutex(5000);
hierarchical_mutex other_mutex(6000);

// 假定其没有锁住任何互斥
int do_low_level_stuff();
// 处于最低层,锁住了low_level_mutex互斥
int low_level_func() {
    std::lock_guard<hierarchical_mutex> lk(low_level_mutex);
    return do_low_level_stuff();
}

void high_level_stuff(int some_param);
// 首先锁住high_level_mutex，再调用low_level_func
// 符合规则,high_level_mutex在10000层级, low_level_mutex在5000层级
void high_level_func() {
    std::lock_guard<hierarchical_mutex> lk(high_level_mutex);
    high_level_stuff(low_level_func());
}
// 符合规则, 可以正常运行
void thread_a() {
    high_level_func();
}

void do_other_stuff();
void other_stuff() {
    high_level_func();
    do_other_stuff();
}

// 先锁住other_mutex层级6000,再调用other_stuff
// other_stuff中先锁住high_level_mutex, 层级10000高于other_mutex层级6000
// 不符合规则，运行期出错
void thread_b() {
    std::lock_guard<hierarchical_mutex> lk(other_mutex);
    other_stuff();
}
```



简单的层级锁实现

```C++
class hierarchical_mutex {
    std::mutex internal_mutex;
    // 当前层级锁的层级
    unsigned long const hierarchy_value;
    // 加锁时备份当前线程层级,unlock时恢复线程层级
    unsigned long previous_hierarchy_value;

    // 存储当前线程层级编号，采用线程专属局部变量thread_local
    // 所有的互斥示例都可以读到该值,同时其值因不同线程而异
    // 使得可以独立检测各线程的行为，各互斥都能判断是否允许当前线程对其加
    // 初始化为unsigned long可表示的最大值ULONG_MAX
    static thread_local unsigned long this_thread_hierarchy_value;
    void check_for_hierarchy_violation() {
        if(this_thread_hierarchy_value <= hierarchy_value) {
            throw std::logic_error("mutex hierarchy violated");
        }
    }

    // 将当前线程层级编号保存到previous_hierarchy_value
    // 当前线程层级设置为该层级锁的层级
    void update_hierarchy_value() {
        previous_hierarchy_value = this_thread_hierarchy_value;
        this_thread_hierarchy_value = hierarchy_value;
    }

public:
    explicit hierarchical_mutex(unsigned long value):
        hierarchy_value(value),
        previous_hierarchy_value(0) {}

    void lock() {
        // 层级规则检查
        check_for_hierarchy_violation();
        internal_mutex.lock();
        // 备份当先线程层级到previous_hierarchy_value,设置当前线程层级
        update_hierarchy_value();
    }

    void unlock() {
        if(this_thread_hierarchy_value != hierarchy_value) {
            throw std::logic_error("mutex hierarchy violated");
        } 
	    // 复原线程层级(复原为lock前当前线程层级)
        this_thread_hierarchy_value = previous_hierarchy_value;
        internal_mutex.unlock();
    }

    bool try_lock() {
        check_for_hierarchy_violation();
        // 加锁失败,不更新线程层级
        if(!internal_mutex.try_lock()) {
            return false;
        }
        update_hierarchy_value();
        return true;
    }
};
thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value(ULONG_MAX);
```



# 保护共享数据的其他工具

## 在初始化过程中保护共享数据

如果我们需要共享一个数据，但是创建它的开销比较大，例如建立数据库连接或分配较大的内存，可以采用延迟初始化(lazy initialization)，等到必须使用的时候再真正创建。且该共享数据进在初始化的过程中需要保护，数据一旦创建便只处于只读的状态，不再存在同步操作。

```C++
std::shared_ptr<some_resource> resource_ptr;
void foo() {
    // 对于单线程是OK的
    // 如果这个共享数据需要被并发访问，需要进行数据保护
    if(!resource_ptr) {
		resource_ptr.reset(new some_resource); 
    }
	resource_ptr->do_something();
}
```

可以采用互斥实现来实现安全的延迟初始化：

```C++
std::mutex resource_mutex;
void foo() {
    // 所有的线程都被迫在此序列化，循序运行
    std::unique_lock<std::mutex> lk(resource_mutex);
    if(!resource_ptr) {
        // 仅有初始化需要保护,大多数场景数据是只读的,不存在同步问题
        // 不需要进行数据保护 
        resource_ptr.reset(new some_resource);
    }
    lk.unlock();
    resource_ptr->do_something();
}
```

上述实现，如果数据被多个线程使用，便无法并发访问，每个线程都需要早互斥锁上等待，而实际上，初始化操作只需要进行一次且初始化完成后数据处于只读状态不需要数据保护。



为了解决上述问题，提出了双重检验锁定模式(double-checked locking pattern)。

```C++
void undefined_behaviour_with_double_checked_locking() {
    // 在无锁条件下检查指针知否为空，如果为空则获取锁
    if (!resource_ptr) { // 1
        std::lock_guard<std::mutex> lk(resource_mutex);
        // 判断指针是否为空和加锁之间存在间隙,可能其他线程在此期间正好完成初始化
        // 再次检查指针是否为空，如果为空则创建
        if (!resource_ptr) { // 2
            resource_ptr.reset(new some_resource); // 3
        }
    }
    resource_ptr->do_something();
}
```

这种实现模式可能会导致**数据竞争问题**，其问题的根源在于：当前线程在锁保护的范围外读指针(步骤1)，如果有一个线程先获取了锁，便可以进入锁保护的范围进行写操作(步骤3)，而读写操作没有同步，便可能会产生数据竞争问题。

| 线程1                      | 线程2                                                        |
| -------------------------- | ------------------------------------------------------------ |
|                            | `!resource_ptr`步骤1读指针                                   |
|                            | `std::lock_guard<std::mutex> lk(resource_mutex)`加锁保护数据 |
|                            | `!resource_ptr` 步骤2再次读指针，判断为空则创建              |
| `!resource_ptr`步骤1读指针 | `resource_ptr.reset(new some_resource)`步骤3写指针           |





C++标准库中提供了`std::once_flag`类和`std::call_once()`函数，以专门处理该情况。令所有的额想成共同调用`std::call_once()`,通过合适的同步机制，确保在该调用返回时，指针初始化由其中某线程安全地唯一完成。使用`std::call_once()`实现上述初始化操作

```C++
std::shared_ptr<some_resource> resource_ptr;
// 存储同步数据, 每个std::once_flag实例对应一次不同的初始化
std::once_flag resource_flag;
void init_resource() {
    resource_ptr.reset(new some_resource);
}

void foo() {
    // 初始化函数准确地被唯一一次调用
    std::call_once(resource_flag,init_resource);
    resource_ptr->do_something();
}
```

相比于采用显式地采用互斥，`std::call_once`函数的额外开销更小，特别是在初始化已经完成的情况下，如果符合场景应该被优先使用。`std::call_once`函数可以与任何函数或可调用对象配合工作。

如果要初始化的数据是某个类的成员，依然可以很方便地实施延迟处世如，例如，利用`std::call_once()`函数对类`X`的数据成员实施线程安全的延迟初始化

```C++
// send_data和receive_data时才进行初始化,将初始化动作延迟
// 借助std::once_flag和std::call_once保证初始化操作只进行一次
class X {
private:
    connection_info connection_details;
    connection_handle connection;
    // 不可复制且不可移动
    std::once_flag connection_init_flag;
    // 进行数据初始化
    void open_connection() {
        connection = connection_manager.open(connection_details);
    }

public:
    X(connection_info const& connection_details_) :
    	connection_details(connection_details_) {}
    void send_data(data_packet const& data) {
         std::call_once(connection_init_flag,&X::open_connection,this);     
 		connection.send_data(data);
    }
    
    data_packet receive_data() {
        std::call_once(connection_init_flag, &X::open_connection, this);      
 		return connection.receive_data();
    }
};
```



如果将其中的局部变量初始化为static数据，便有可能会引起条件竞争。由于早前的C++标准规定，只要控制流程第一次遇到静态数据的声明语句，变量即进行初始化。若多个线程同时调用同一函数，而它含有静态数据，则任意线程均可能首先到达其声明处，这就形成了条件竞争的隐患。因此早前C++标准(不支持C++11的)会导致冲多线程都认定自己是第一个进行初始化的，从而都尝试初始化变量，而此时可能某个线程正在只进行初始化，又存在其他的此线程试图使用它，从而导致了数据竞争。C++11解决了这个问题，C++11规定初始化只会在某一线程上单独发生，在初始化完成之前，其他线程不会越过静态数据的声明而继续运行。

```C++
class my_class;
// 线程安全的初始化，C++11标准保证其正确性
my_class& get_my_class_instance() {
    static my_class instance;
    return instance;
}
```

仅在初始化过程中保护共享数据只是一种特例，更普遍的情况是保护那些甚少更新的数据结构。大多数时候，这些数据结构都处于只读状态，因此可被多个线程并发访问，但它们偶尔也需要更新，我们需要一种保护机制专门处理这种场景。

