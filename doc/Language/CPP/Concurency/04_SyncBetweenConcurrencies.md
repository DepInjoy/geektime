# 使用future等待一次性事件发生

C++标准程序库使用future来模拟这类一次性事件：若线程需等待某个特定的一次性事件发生，则会以恰当的方式取得一个代表目标事件的`future`，接着，该县城就能够一边执行其他任务，一遍在future上等待；同时，它以较短暂的间隔反复查验目标事件是否已经发生。这个线程也可以转换运行模式，先不等目标事件发生，直接暂缓当前任务，而切换到别的任务，及至必要时，才回头等待`future`准备就绪。`future`可能与数据关联，也可能未关联。一旦目标事件发生，其`future`即进入就绪状态，无法重置。

C++标准程序库有两种`future`，分别由两个类模板实现，其声明位于标准库的头文件`<future>`内：独占`future(unique future`，即`std::future<>`)和共享`future`(shared future，即`std::shared_future<>`）。同一事件仅仅允许关联唯一一个`std::future`实例，但可以关联多个`std::shared_future`实例，只要目标事件发生，与后者关联的所有实例会同时就绪，并且，它们全都可以访问与该目标事件关联的任何数据。如果没有关联数据，我们应使用特化的模板`std::future<void>`和`std::shared_future<void>`。`future`能用于线程间通信，但是`future`对象本身不提供同步访问。若多个线程需访问同一个`future`对象，必须用互斥或其他同步方式进行保护。

C++的并发技术规约在`std::experimental`名字空间中给出了上述类模板的扩展版本：`std::experimental::future<>`和`std::experimental::shared_future<>`。它们的行为与std名字空间中的版本一致，但具备额外的成员函数，可提供更多功能。这里的`experimental`意在强调其中所含的类和函数尚未被C++标准正式采纳。



## 从后台任务返回值

例如，运用`std::future`取得异步任务的函数返回值

```C++
#include <future>
#include <iostream>
int find_the_answer_to_ltuae();
void do_other_stuff();

int main() {
    // 通过std::async获得std::future对象
    // 运行函数一旦完成, 其返回值就由该对象最后持有
    // 只要在这个future对象上调用get()当前线程便会阻塞，以便future准备妥当且返回该值
    std::future<int> the_answer = std::async(find_the_answer_to_ltuae);
    do_other_stuff();
    std::cout<<"The answer is "<<the_answer.get()<<std::endl;
}
```

向任务函数传递参数

例如，通过`std::async()`向任务函数传递参数

```C++
#include <string>
#include <future>
struct X {
    void foo(int,std::string const&);
    std::string bar(std::string const&);
};

X x;
// 调用p->foo(42,"hello"),其中p的值是&x，即x的地址
auto f1=std::async(&X::foo, &x, 42, "hello");
// 调用tmpx.bar("goodbye"),其中tmpx是x的副本
auto f2=std::async(&X::bar, x, "goodbye");

struct Y {
    double operator()(double);
}; 
Y y;
// 调用tmpy(3.141). 其中,由Y()生成一个匿名变量，传递给std::async()，进而发生移动构造
// 在std::async()内部产生对象tmpy，在tmpy上执行Y::operator()(3.141) 
auto f3=std::async(Y(), 3.141);
// 调用y(2.718)
auto f4=std::async(std::ref(y), 2.718);

X baz(X&);
// 调用baz(x)
std::async(baz,std::ref(x));

class move_only {
public:
    move_only();
    move_only(move_only&&)
    move_only(move_only const&) = delete;
    move_only& operator=(move_only&&);
    move_only& operator=(move_only const&) = delete;
    void operator()();
}; 
// 调用tmp()，其中tmp等价于std::move(move_only())
auto f5=std::async(move_only());
```



默认情况下，`std::async()`会自行决定--等待`future`时，是启动新线程或者是同步执行任务。我们还可以给`std::async`传递参数来执行采用哪种方式运行，相关定义如下：

```C++
async(std::launch policy, Function&& f, Args&&... args );
```

该参数的类型是`std::launch`，其可能的取值是：

- `std::launch::deferred `：在当前线程上延后调用任务函数，等到在`future`上调用了`wait()`或`get()`，任务函数才会执行。如果延后调用任务函数，该任务函数有可能永远都不会被调用。
- `std::launch::async`：指定必须另外开启专属的线程，在其上运行任务函数.
- `std::launch::deferred | std::launch::async`：表示由`std::async()`的实现自行选择运行方式，这是该参数的默认值。

```C++
// 运行新线程, 在其上运行任务函数
auto f6=std::async(std::launch::async,Y(),1.2);
// 在当前线程延后调用任务函数
// 等在future上调用wait()或get(),才会运行该任务函数
auto f7=std::async(std::launch::deferred, baz, std::ref(x)); // 2
// std::sync()实现自行选择运行方式
auto f8=std::async(
   std::launch::deferred | std::launch::async,
   baz, std::ref(x));

// std::sync()实现自行选择运行方式
auto f9=std::async(baz,std::ref(x));

// 2处被延后调用的函数，在这里才被运行
f7.wait();
```



使`std::future`和任务关联并非唯一的方法：

- 运用类模板`std::packaged_task<>`的实例，也能将任务包装起来
- 利用`std::promise<>`类模板编写代码，显式地异步求值

## 关联future实例和任务



## 创建std::promise显式地异步求值



