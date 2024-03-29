1. 为什么需要原子操作？
2. 如何实现原子操作：借助原子指令直接实现，也可以通过互斥操作借助程序内部锁来实现(损失性能)
2. 原子操作由什么构成？
3. 如何利用原子操作强制操作服从预定次序？ 

如果两个线程同时访问同一内存区域，任一线程更改数据都可能会引发条件竞争，<b><font color=00FF80>为避免条件竞争需要强制两个线程按照一定次序访问</font></b>，有两种实现方式：运用互斥保证访问次序；<b><font color=00FF80>利用原子操作的同步性质来保证访问次序。</font></b>目前绝大多数实际的并行CPU都采用弱一致性，对于多个线程要访问相同内存单元的位置，需要适当插入内存同步指令来使线程程看到一致的数据。

---

<b><font color=FFC300>并行计算机中，每个CPU单元都可能含有Cache，需要考虑所有CPU之间的Cache数据同步机制。</font></b> 目前最常用的方法是基于目录的Cache一致性协议。当一个CPU写内存时，要查找目录表，如果该内存单元在其他CPU中含有备份，则向其他CPU发送广播通知。目标CPU收到通知后，更新自身包含的Cache数据。Cache目录一致既适用于SMP也适用于NUMA。Cache目录一致性实现了多个CPU之间的Cache同步，但是不同计算机对Cache更新通知的时序规定了不同的原则。

- 强一致性：系统中所有更新Cache的通知要执行结束，才允许各CPU执行后续的访存指令。这种方式使所有处理器核之间严格保证Cache一致性，但是会使各CPU花费大量时间等待Cache通知结束，从而降低了系统性能。
- 弱一致性：各CPU不需要等待所有Cache通知执行结束，就可以执行访存指令。在这种情况下，CPU硬件不维护所有Cache的强制一致性，某一个CPU写内存的行为可能不会及时通知到所有其他CPU，这时不同的CPU会在Cache中读取出不同的数值。<b><font color=FFC300>如果程序员觉得在有些程序中必须保证强一致性，可以调用CPU提供的一条“内存同步指令”，强行使CPU等待所有Cache更新结束。</font></b>

<b><font color=FFC300>目前绝大多数实际的并行CPU都采用弱一致性。弱一致性让程序员承担了维护代价，</font></b>但是性能比强一致性要高很多倍。程序员在编写并行算法时，<b><font color=FFC300>对于多个线程要访问相同内存单元的位置，只需要适当插入内存同步指令来使线程程看到一致的数据。</font></b>
<br/>

<b><font color=#FFC300>原子指令(Atomic Instruction)用于在多个CPU之间维护同步关系。</font></b>在一些科学计算问题中，通过并行算法把子问题分配到多个CPU上执行，但是各个子问题之间存在合作关系，因此需要硬件机制来实现多个CPU之间的同步。一个典型的同步例子是“原子加1”问题。例如，一个CPU要对内存单元M中的数据加1，这个动作需要3条指令来完成：
1. 读M的值到寄存器R
2. 对R执行加1运算
3. 把R的值写回内存单元M。
<center>
    <img src="./img/Atomic_CPU_Sync.png">
</center>
如果计算机中只有一个CPU，执行上面3条指令不会有任何问题。但是如果CPU有两个，则可能在一个CPU执行过程中，另一个CPU也执行这3条指令，最后M的结果不是增加2而是增加1。原子指令可以实现一个CPU独占执行时间。使用原子指令把连续多条指令包含起来，计算机保证只有一个CPU处于执行状态，其他CPU必须等待原子指令结束才能继续执行
---


# 内存模型
内存模型精确定义了基础构建单元应当如何运转，牵涉两个方面：基本结构和并发。基本结构关系到整个程序在内存中的布局，这对并发很重要，特别是分析底层原子操作。对于C++，基本结构就是对象和内存区域。

<center>
    <img src="./img/5_1_StructRegionStorage.png">
    <div>将Struct分解为对象和内存区域<</div>
</center>
<br/>

`bf3`是0宽度位域(其变量名被注释掉，因为0宽度位域必须匿名)，与`bf4`彻底分离，将`bf4`排除在`bf3`的内存区域之外，但`bf3`实际上并不占有任何内存区域。

---
1. 每一个变量都是一个对象，其成员变量也是对象。
2. 每个对象至少占用一个内存区域(Memory Location)。
3. 若变量属于内建基本类型(如`int`或`char`)，则不论其大小，都占用一块内存区域(且仅此一块)，即便它们的位置相邻或它们是数列中的元素。
4. 相邻位域属于同一内存区域。(在C++和C中规定，宽度为0的一个未命名位域强制下一位域对齐到其下一type边界)
---

C++中所有与多线程相关的事项都会牵涉内存区域。如果两个线程各自访问分离的内存区域，一切运行良好；反之，<b><font color=#FFC300>如果两个线程访问同一内存区域就需要注意，可能出现问题。</font></b>如果没有线程更新内存区域，则不必在意，只读数据无须保护或同步。<b><font color=#FFC300>任一线程改动数据都有可能引发条件竞争。要避免条件竞争，就必须强制两个线程按一定的次序访问。</font></b>该次序可以固定不变，即某一访问总是先于另一个；也可变动，即随程序运行而间隔轮换访问的次序，但必须保证访问次序条理清晰分明。

- <b><font color=#FFC300>运用互斥保证访问次序清晰分明。</font></b>若在两个访问发生前，先行锁定相关互斥，那么每次仅容许一个线程访问目标内存区域，遂一个访问必然先于另一个(通常无从预知具体哪个访问在前)。
- <b><font color=#FFC300>利用原子操作的同步性质</font></b>，在目标内存区域(或相关内存区域)采取原子操作，强制两个线程遵从一定的访问次序。


# 原子操作及其类别

标准原子类型的定义位于头文件`<atomic>`内，标准原子对象不具备拷贝构造函数或拷贝赋制操作符。借助互斥保护，模拟出标准的原子类型。`is_lock_free()`成员函数准许使用者判定某一给定类型上的操作是能由原子指令(atomic instruction)直接实现(`x.is_lock_free()`返回`true`），还是借助编译器和程序库内部锁实现(`x.is_lock_free()`返回`false`)。

```C++
// 原子操作有两种实现方式
//      1. 采用互斥保护借助编译器和程序库的内部所实现
//      2. 原子指令直接实现
// 实现检查原子变量是否是采取原子指令实现(返回true)
bool is_lock_free()
```

原子操作的关键用途是取代需要互斥的同步方式，假如原子操作本身也在内部使用了互斥，就很可能无法达到所期望的性能提升，而更好的做法是采用基于互斥的方式，该方式更加直观且不易出错。

C++程序库专门为此提供了一组宏:
```C++
ATOMIC_BOOL_LOCK_FREE
ATOMIC_CHAR_LOCK_FREE、ATOMIC_CHAR16_T_LOCK_FREE
ATOMIC_CHAR32_T_LOCK_FREE、ATOMIC_WCHAR_T_LOCK_FREE
ATOMIC_SHORT_LOCK_FREE、ATOMIC_INT_LOCK_FREE
ATOMIC_LONG_LOCK_FREE、ATOMIC_LLONG_LOCK_FREE和ATOMIC_POINTER_LOCK_FREE
```
实现由不同整数类型特化而成的各种原子类型，在编译期判定其是否属于无锁数据结构。从C++17开始，全部原子类型都含有一个静态常量表达式成员变量(static constexpr member variable)，形如`X::is_always_lock_free`，功能与那些宏相同：考察编译生成的一个特定版本的程序，当且仅当在所有支持该程序运行的硬件上，原子类型X全都以无锁结构形式实现，该成员变量的值才为true。

```C++
void store( T desired, std::memory_order order = std::memory_order_seq_cst ) noexcept;
void store( T desired, std::memory_order order = std::memory_order_seq_cst ) volatile noexcept;

T load( std::memory_order order = std::memory_order_seq_cst ) const noexcept;
T load( std::memory_order order = std::memory_order_seq_cst ) const volatile noexcept;
 
 
T exchange(T desired, std::memory_order order = std::memory_order_seq_cst ) noexcept;
T exchange(T desired, std::memory_order order = std::memory_order_seq_cst ) volatile noexcept;

bool compare_exchange_weak( T& expected, T desired, std::memory_order success,
                            std::memory_order failure ) noexcept;
bool compare_exchange_strong( T& expected, T desired, std::memory_order success,
                              std::memory_order failure ) noexcept;

 
T fetch_and(T arg, std::memory_order order = std::memory_order_seq_cst ) noexcept;
T fetch_and(T arg, std::memory_order order = std::memory_order_seq_cst ) volatile noexcept;

T fetch_or( T arg, std::memory_order order = std::memory_order_seq_cst ) noexcept;
T fetch_or( T arg, std::memory_order order = std::memory_order_seq_cst ) volatile noexcept;
```



对于原子类型上的每一种操作，都可以提供额外的参数，从枚举类`std::memory_order`取值，用于设定所需的内存次序语义（memory-ordering semantics）。操作的类别决定了内存次序所准许的取值，则默认采用最严格的内存次序，即`std::memory_order_seq_cst`。枚举类`std::memory_order`具有6个可能的值，包括

1. `std::memory_order_relaxed`
2. `std::memory_order_release`
3. `std::memory_order_seq_cst`
4. `std:: memory_order_acquire`
5. `std::memory_order_consume`
6. `std::memory_order_acq_rel`

只代表3种模式：先后一致次序(`memory_order_seq_cst`)、获取-释放次序(`memory_order_consume`、`memory_order_acquire`、`memory_order_release`和`memory_order_acq_rel`)、宽松次序(`memory_order_relaxed`)。

内存次序根据操作类别被划分为3类：
1. 存储(`store`)操作，可选用的内存次序有`std::memory_order_relaxed`、`std::memory_order_release`或`std::memory_order_seq_cst`。
2. 载入(`load`)操作，可选用的内存次序有`std::memory_order_relaxed`、`std::memory_order_consume`、`std::memory_order_acquire`或`std::memory_order_seq_cst`。
3. 读-改-写(`read-modify-write`)操作，可选用内存次序`std::memory_order_relaxed``std::memory_order_consume`、`std::memory_order_acquire`、``std::memory_order_release`、`std::memory_order_acq_rel`或`std::memory_order_seq_cst`。

## 操作std::atomic_flag
`std::atomic_flag`原子类型不提供`is_lock_free()`成员函数，是简单的布尔标志，必须采取无锁操作。只要利用这种简单的无锁布尔标志，我们就能实现一个简易的锁，进而基于该锁实现其他所有原子类型。这里说简单，确实如此：类型`std::atomic_flag`的对象在初始化时清零，可通过成员函数`test_and_set()`查值并设置成立，或由`clear()`清零，只有这两个操作。
```C++

```
# 同步操作和强制次序

## 依据原子对象的当前值决定是否保存新值

这个操作称为比较-交换(compare-exchange)，实现形式是成员函数`compare_exchange_weak()`和`compare_exchange_strong()`, 比较-交换操作是原子类型的编程基石。给定一个期望值(expected)，原子变量将它和自身的值比较，如果相等，就存入另一给定值(desired)；否则，用存储在this变量中的值更新期望值所属变量。比较-交换函数返回布尔类型，如果完成了保存动作(前提是两者相等)，则操作成功，返回ture；反之操作失败，返回false。

```C++
bool compare_exchange_weak(T& expected, T desired,
        std::memory_order success, std::memory_order failure) noexcept;
bool compare_exchange_weak( T& expected, T desired,
        std::memory_order success, std::memory_order failure ) volatile noexcept;


bool compare_exchange_strong(T& expected, T desired,
        std::memory_order success, std::memory_order failure) noexcept;
bool compare_exchange_strong( T& expected, T desired,
        std::memory_order success, std::memory_order failure ) volatile noexcept;
```

```C++
bool compare_exchange_weak(T& expected, T desired,
        std::memory_order order = std::memory_order_seq_cst) noexcept;
bool compare_exchange_weak(T& expected, T desired,
        std::memory_order order = std::memory_order_seq_cst) volatile noexcept;

bool compare_exchange_strong(T& expected, T desired,
        std::memory_order order = std::memory_order_seq_cst) noexcept;
bool compare_exchange_strong(T& expected, T desired,
        std::memory_order order = std::memory_order_seq_cst) volatile noexcept;
```

> Atomically compares the object representation(until C++20) value representation(since C++20) of *this with that of expected. If those are bitwise-equal, replaces the former with desired (performs read-modify-write operation). Otherwise, loads the actual value stored in *this into expected (performs load operation).
>
> 来自[cppreference:atomic](https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange)
>
> 也就是说，原子地将this的对象表示(直到C++20)或值表示(自C++20起)与expected进行比较。如果它们在位级别上相等(bitwise-equal)，则用desired替换前者(执行读-修改-写操作)。否则，将存储在this中的实际值加载到expected中(执行加载操作)。

对于`compare_exchange_weak()`，即使原子变量的值等于期望值，保存动作还是有可能失败，此时，原子变量维持原值不变，`compare_exchange_weak()`返回false。这是由于原子化的比较-交换必须由一条指令单独完成，而某些处理器没有这种指令，无从保证该操作按原子化方式完成。要实现比较-交换，负责的线程则须改为连续运行一系列指令，但在这些计算机上，只要出现线程数量多于处理器数量的情形，线程就有可能执行到中途因系统调度而切出，导致操作失败。这种计算机最有可能引发上述的保存失败，称之为**佯败(spurious failure)**。其败因不是变量值本身存在问题，而是函数执行时机不对。因为`compare_exchange_weak()`可能佯败，所以它往往必须配合循环使用。

```C++
bool expected = false;
extern atomic<bool> b; //由其他源文件的代码设定变量的值
while (!b.compare_exchange_weak(expected, true) && !expected);
```

只有当原子变量的值不符合预期时，`compare_exchange_strong()`才返回`false`，这让我们可以明确知道变量是否成功修改，或者是否存在另一线程抢先切入而导致佯败，从而能够摆脱上例所示的循环。

假如经过简单计算就能得出要保存的值，而在某些硬件平台上，虽然使用`compare_exchange_weak()`可能导致佯败，但改用`compare_exchange_strong()`却会形成双重嵌套循环（因`compare_exchange_strong()`自身内部含有一个循环），那么采用`compare_exchange_weak()`比较有利于性能。反之，如果存入的值需要耗时的计算，选择`compare_exchange_strong()`则更加合理。因为只要预期值没有变化，就可避免重复计算。就``std::atomic<bool>`而言，这并不是很重要，毕竟只有两种可能的值，但对于体积较大的原子类型，这两种处理的区别很大。

