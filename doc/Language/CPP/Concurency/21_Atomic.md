1. 如果两个线程同时访问统一内存区域，任一线程更改数据都可能会引发条件竞争，为避免条件竞争需要强制两个线程按照一定次序访问，有两种实现方式：运用互斥保证访问次序；利用原子操作的同步性质来保证访问次序。


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

```C++
// 原子操作有两种实现方式
//      1. 采用互斥保护借助编译器和程序库的内部所实现
//      2. 原子指令直接实现
// 实现检查原子变量是否是采取原子指令实现(返回true)
bool is_lock_free()
 
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

内存次序根据操作类别被划分为3类：

1. 存储(`store`)操作，可选用的内存次序有`std::memory_order_relaxed`、`std::memory_order_release`或`std::memory_order_seq_cst`。
2. 载入(`load`)操作，可选用的内存次序有`std::memory_order_relaxed`、`std::memory_order_consume`、`std::memory_order_acquire`或`std::memory_order_seq_cst`。
3. 读-改-写(`read-modify-write`)操作，可选用内存次序`std::memory_order_relaxed``std::memory_order_consume`、`std::memory_order_acquire`、``std::memory_order_release`、`std::memory_order_acq_rel`或`std::memory_order_seq_cst`。
