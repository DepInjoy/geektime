# 信号量

信号量代表一定的资源数量，可以根据当前资源的数量按需唤醒指定数量的资源消费者线程，资源消费者线程一旦获取信号量，就会让资源减少指定的数量，如果资源数量减少为0，则消费者线程将全部处于挂起状态；当有新的资源到来时，消费者线程将继续被唤醒。

## C++11借助
初始化和销毁的API

```C++
#include <semaphore.h>

/**
 * 用于初始化信号量
 *      @pshared: 表示信号量是否可以被共享
 *          0: 只能在同一个进程的多个线程间共享
 *          非0: 可以在多个进程之间共享
 *      @value:   设置信号量的初始值
 * @return  成功返回0，失败返回-1，一般可以不关注该返回值
*/
int sem_init(sem_t* sem, int pshared, unsigned int value);
// 销毁信号量
int sem_destroy(sem_t*sem);
```

```C++
// 阻塞线程调用,直到信号量对象的资源计数大于0时被唤醒,唤醒后将资源计数递减1,然后立即返回
int sem_wait(sem_t* sem);
// sem_wait的非阻塞版，如果资源计数大于0，成功返回0
// 如果当前信号量对象的资源计数等于0，则sem_trywait函数会立即返回
// 不阻塞调用线程，返回值是-1，错误码errno被设置成EAGAIN
int sem_trywait(sem_t*sem);
// sem_wait等待一段时间，abs_timeout是等待时间，如果资源计数大于0，成功返回0
// 如果超时返回，返回值为-1，错误码errno是ETIMEDOUT
int sem_timedwait(sem_t* sem, const struct timespec* abs_timeout);
```

```C++
// 将信号量的资源计数递增1，并解锁该信号量对象
// 使得唤醒因sem_wait函数被阻塞的其他线程
int sem_post(sem_t* sem);
```

[C++11信号量实现生产者消费者模型](./ThreadSync/semaphore_11.cpp)

## C++20信号量

C++20提供了`std::counting_semaphore`和`std::binary_semaphore`支持信号量。

> 信号量亦常用于发信/提醒而非互斥，通过初始化该信号量为 0 从而阻塞尝试 acquire() 的接收者，直至提醒者通过调用 release(n) 通知，在这一点可把信号量当作 `std::condition_variable` 的替用品，通常它有更好的性能。
>
> 1. `counting_semaphore` 是一个轻量同步元件，能控制对共享资源的访问。不同于`std::mutex`、 `counting_semaphore` 允许同一资源有多于一个同时访问，至少允许 `LeastMaxValue` 个同时的访问者若`LeastMaxValue` 为负则程序为谬构。
>
>
> 2. `binary_semaphore` 是`std::counting_semaphore`的特化的别名，其`LeastMaxValue` 为 1 。实现可能将 `binary_semaphore` 实现得比`std::counting_semaphore`的默认实现更高效。
>
>     ​						-- 来自[counting_semaphore](https://zh.cppreference.com/w/cpp/thread/counting_semaphore)

```C++
// desired初始化counting_semaphore的计数器的值
constexpr explicit counting_semaphore(std::ptrdiff_t desired);

// 原子地将内部计数器的值增加update
void release(std::ptrdiff_t update = 1);

// 若内部计数器大于0则尝试将它减少1;否则阻塞直至它大于0且能成功减少内部计数器
void acquire()
// 若内部计数器大于0则尝试原子地将它减少1;不阻塞
// 若减少内部计数器则为true, 否则为false
bool try_acquire() noexcept;

// 若内部计数器大于0则尝试原子地将它减少1;否则阻塞直至它大于0且能成功地减少内部计数器
// 或等待已经超出rel_time,可能会抛出std::system_error异常
template<class Rep, class Period>
bool try_acquire_for(const std::chrono::duration<Rep, Period>& rel_time);

// 若内部计数器大于0则尝试原子地将它减少1;否则阻塞直至它大于0且能成功地减少内部计数器
// 或已经经过 abs_time 时间点, 可能会抛出std::system_error异常
template<class Clock, class Duration>
bool try_acquire_until( const std::chrono::time_point<Clock, Duration>& abs_time );
```



[C++11信号量实现生产者消费者模型](./ThreadSync/semaphore_20.cpp)

# 条件变量

条件变量采用通知-唤醒模型。

> The `condition_variable` class is a synchronization primitive used with a std::mutex to block one or more threads until another thread both modifies a shared variable (the condition) and notifies the `condition_variable`.
>
> The thread that intends to modify the shared variable must:
>
> 1. Acquire a `std::mutex` (typically via std::lock_guard)
> 2. Modify the shared variable while the lock is owned
> 3. Call notify_one or notify_all on the `std::condition_variable` (can be done after releasing the lock)
>
>   -- 来自[std::condition_variable](https://en.cppreference.com/w/cpp/thread/condition_variable)

翻译一下的大体意思是：`condition_variable`是和`std::mutex`一起使用的同步原语，`std::mutex`用于阻塞一个或多个线程，直到另外一个线程修改共享变量(`condition`)并通知`condition_variable`。

想要修改共享变量的线程必须：

1. 获取`std::mutex`(通常通过std::lock_guard)
2. 持有锁时，修改共享变量
3. 在`std::condition_variable`上调用notify_one或notify_all(可以在释放锁之后执行)



下面这个引用[2]中的描述，实现一遍之后会觉得更易理解

> 什么是条件变量呢，引用APUE中的一句话:
>
> Condition variables are another synchronization mechanism available to threads. These synchronization objects provide a place for threads to rendezvous. When used with mutexes, condition variables allow threads to wait in a race-free way for arbitrary conditions to occur.
>
> 条件变量是线程的另外一种有效同步机制。这些同步对象为线程提供了交互的场所（一个线程给另外的一个或者多个线程发送消息），我们指定在条件变量这个地方发生，一个线程用于修改这个变量使其满足其它线程继续往下执行的条件，其它线程则等待接收条件已经发生改变的信号。当条件变量同互斥锁一起使用时，条件变量允许线程以一种无竞争的方式等待任意条件的发生。



从下面几个角度来理解条件变量

1. 为什么需要条件变量(适用场景)
2. 为什么条件变量和互斥体一定要结合使用？(确保解锁和等待在一个原子操作中)
3. 如何使用条件变量？
4. 什么是虚假唤醒？为什么会出现虚假唤醒？
5. 条件变量的信号丢失(条件变量唤醒时，相关线程没有调用`wait`)



## 适用场景

在实际应用中，我们常常会有类似如下的需求
```C++
// 以下是伪代码, m的类型是pthread_mutex_t, 已经初始化过了
int WaitForTrue() {
    do {
        pthread_mutex_lock(&m);
        // 验证condition是否为true
        // 解锁, 让其他线程有机会改变condition   
        pthread_mutex_unlock(&m);     // 睡眠n秒        
        sleep(n);
    } while (condition is false);

    return 1;
}
```
```plantuml
@startuml
:加锁进入临界区;


@enduml
```
这段逻辑用于反复判断一个多线程的共享条件是否满足，一直到该条件满足为止。由于该条件被多个线程操作，因此在每次判断之前都需要进行加锁操作，判断完毕后需要进行解锁操作。该逻辑存在严重的效率问题，假设解锁离开临界区后，其他线程修改了条件导致条件满足，则此时程序仍然需要睡眠n秒才能得到反馈。因此我们需要这样一种机制：某个线程A在条件不满足的情况下主动让出互斥体，让其他线程操作，线程A在此处等待条件满足；一旦条件满足，线程A就可以被立刻唤醒。

[C++多线程并发(三) --- 线程同步之条件变量](https://blog.csdn.net/m0_37621078/article/details/89766449)这个博客用代码的方式形象展示了上述过程。



## 条件变量和互斥体结合的原因

为什么条件变量和互斥体一定要结合使用？假定条件变量和互斥体不结合，下面是伪代码

```C++
//m的类型是pthread_mutex_t，并且已经初始化过了，cv是条件变量
pthread_mutex_lock(&m);
while(condition_is_false) {
    pthread_mutex_unlock(&m);
    //解锁之后，等待之前，可能条件已经满足，信号已经发出，但是该信号可能被错过
    cond_wait(&cv);
    pthread_mutex_lock(&m);
}
```

在以上代码中，假设线程A在执行完第5行代码后CPU时间片被剥夺，此时另一个线程B获得该互斥体对象 m，然后发送条件信号，等线程A重新获得时间片后，由于该信号已经被错过，可能会导致线程A在代码第7行无限阻塞下去。

问题的根源是释放互斥体对象与条件变量等待唤醒不是原子操作，即解锁和等待这两个步骤必须在同一个原子操作中，才能确保`cond_wait`在唤醒之前不会有其他线程获得这个互斥体对象。



## 条件变量使用

```C++
// 阻塞当前线程,直到唤醒条件变量
void wait (unique_lock<mutex>& lck);

// 阻塞当前线程,直到唤醒条件变量且stop_waiting条件被满足
template <class Predicate>
void wait (unique_lock<mutex>& lck, Predicate stop_waiting);
```

```C++
// 通知当前正在等待此条件的一个线程
// 如果没有线程在等待, 不执行任何操作
// 如果有多个线程在等待, 随机通知一个线程
void notify_one() noexcept;

// 通知所有线程(类似于广播), 如果没有线程在等待，则函数不执行任何操作
void notify_all() noexcept;
```



[生产者消费者模型 1对1](./ThreadSync/00_ConditionVariable_1to1.cpp)
[生产者消费者模型 1对多](./ThreadSync/00_ConditionVariable_1toMore.cpp)



条件变量最关键是需要弄清楚`pthread_cond_wait`在条件满足与不满足时的两种行为，这是难点和重点。

1. `pthread_cond_wait`函数阻塞时，会释放其绑定的互斥体并阻塞线程。因此在调用该函数前应该对互斥体加锁。
2. 收到条件信号时，`pthread_cond_wait`会返回并对其绑定的互斥体进行加锁，因此在其下面一定有互斥体解锁。



## 条件变量的虚假唤醒

将互斥体和条件变量配合使用的地方，使用了while语句，条件变量醒来之后再次判断条件是否满足：

```C++
// 使用while语句在条件变量醒来之后再次判断条件是否满足
// 不能使用 if (tasks.empty())
while (tasks.empty()) {
    pthread_cond_wait(&mycv, &mymutex);
}
```

这里不得不这样做，这是因为：操作系统可能在某些情况下唤醒条件变量，也就是说存在没有其他线程向条件变量发送信号，但等待此条件变量的线程有可能醒来的情形，将条件变量的这种行为称为虚假唤醒(spurious wakeup)。这里将条件(`tasks.empty()`为true)放在while循环中以为着不仅要唤醒条件变量还必须满足条件，程序才可以执行正常的逻辑。

那么为什么会存在虚假唤醒呢？

1. `pthread_cond_wait`是`futex`系统调用，属于阻塞型的系统调用，当系统调用被信号中断时，会返回-1，并且把`errno`错误码置为`EINTR`。很多这种系统调用在被信号中断后，都会再次调用一次这个函数，其代码如下

    ```C++
    pid_t r_wait(int *stat_loc) {
        int retval;
        //wait函数因为被信号中断导致调用失败，会返回-1，错误码是EINTR
        //注意：这里的while循环体是一条空语句
        while(((retval = wait(stat_loc)) == -1 &&(errno == EINTR));
        return retval;
    }
    ```

    假设 `pthread_cond_wait`函数被信号中断，则在`pthread_cond_wait `函数返回之后，到重新调用之前，`pthread_cond_signal `或`pthread_cond_broadcast`函数可能已被调用。一旦错失该信号，则可能由于条件信号不再产生，再次调用`pthread_cond_wait`函数将导致程序无限等待。为了避免这种情况发生，虚假唤醒，而不是再次调用`pthread_cond_wait`函数，以免陷入无穷等待中。

2. 此外，还存在一些情况：在条件满足时发送信号，但等到调用`pthread_cond_wait`函数的线程得到CPU时间片时，条件又再次不满足了。



## 条件变量信号丢失

如果一个条件变量信号在产生时(调用`pthread_cond_signal`或`pthread_cond_broadcast`)，没有相关线程调用 `pthread_cond_wait`捕获该信号，该信号就会永久丢失，再次调用`pthread_cond_wait`会导致永久阻塞。



# 参考资料

1. C++服务器开发精髓
2. [C++多线程并发(三) --- 线程同步之条件变量](https://blog.csdn.net/m0_37621078/article/details/89766449)