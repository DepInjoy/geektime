# 动态分配和释放内存
在自由空间分配的内存是无名的, new无法为其分配的对象命名，而是返回一个指向该对象的指针
```C++
// pi只想一个动态分配的，未初始化的无名对象
// 在自由空间构造一个int型对象,并返回指向该对象的指针
int *pi =  new int;
```
默认情况下，动态分配的对象是默认初始化的，这意味着内置类型或组合类型的对象的值将是未定义的，而类类型对象将用默认构造函数进行初始化。
```C++
// 初始化为空string
string *ps = new string;

// pi指向一个未初始化的int
int *pi = new int;
```

为了防止内存耗尽，在动态内存使用完毕后，必须将其归还给系统。我们通过delete表达式(delete expression)来将动态内存归还给系统。
```C++
delete p;
```

---
一旦程序用光了它所有可用的内存，new表达式就会失败。默认情况下，如果new不能分配所要求的内存空间，它会抛出一个类型为`bad_alloc`的异常,可以改变使用new的方式来阻止它抛出异常：
```C++
// 如果分配失败，new抛出std::bad_alloc异常
int *p1 =  new int;

// 如果分配失败, new返回空指针
int*p2 = new(nothrow) int;
```
称这种形式的new为定位new(placement new), 定位new表达式允许我们向new传递额外的参数。

# 智能指针

新标准库提供的这两种智能指针的区别在于管理底层指针的方式：

- `unique_ptr`独占所指向的对象，支持将资源转移给其他的`unique_ptr`对象。当拥有所有权的`unique_ptr`对象析构时，资源即被释放。

- `shared_ptr`允许多个指针指向同一个对象。每个`shared_ptr`都有一个关联的计数器，通常称其为引用计数(reference count)。无论何时拷贝一个`shared_ptr`，计数器都会递增。一旦`shared_ptr`的计数器变为0，自动释放自己所管理的对象.
- `weak_ptr`伴随类，它是一种弱引用，指向`shared_ptr`所管理的对象，虽然能访问资源但却不享有资源的所有权，不影响资源的引用计数。有可能资源已被释放，但`weak_ptr`仍然存在，每次访问资源时都需要判断资源是否有效。

这三种类型都定义在`memory`头文件中。



### 循环引用

循环引用就是两个对象相互使用`shared_ptr`指向对方，最终导致了内存泄漏。[循环引用测试用例实现](code/03_shared_ptr_loop_reference.cpp)，有两种方式可以解决循环引用

1. 其中一个对象采用`std::weak_ptr`采取弱引用，确保可以访问共享资源，但不改变资源的引用测试

   ```C++
   class Son {
   public:
       // 采用std::weak_ptr避免shared_ptr循环引用
       std::weak_ptr<Father>       father_;
   
       		......
   };
   ```

2. 手动释放成员，规避循环引用，确保资源可以释放。

   ```C++
       // 在main函数增加释放成员调用
   	// 在手动释放成员,避免循环引用
       son->father_.reset();
       father->son_.reset();
   ```

   