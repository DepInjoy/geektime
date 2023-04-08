C++标准库就能使线程管控的大部分工作变得相对简单：如果给定一个线程，只要令std::thread对象与之关联，就能管控该线程。那么：
1. 怎么启动线程？启动时如何向线程函数传递参数？
2. 怎么查验它们是否已经结束？
3. 怎么监测其运行状态？
4. 如何把线程的归属权从某个std::thread对象转移给另一个？
5. 如何选择合适数量的线程以及如何识别特定的线程？
# 线程基本管控
每个C++程序都含有至少一个线程，即运行main()的线程，它由C++运行时(C++ runtime)系统启动。随后，程序可以发起更多线程，它们以别的函数作为入口(entry point)。这些新线程连同起始线程并发运行。当`main()`返回时，程序就会退出；同样，当入口函数返回时，对应的线程随之终结。

## 发起线程
线程通过构建std::thread对象而启动，该对象指明线程要运行的任务。最简单的任务就是运行一个普通函数，返回空，且不接收参数。函数在自己的线程上运行，等它一返回，线程即随之终止。复杂任务可以由函数对象(function object)表示，还接收参数，并且在运行过程中，经由某种消息系统协调，按照指定执行一系列独立操作，只有收到某指示信号(依然经由消息系统接收)时，线程才会停止。
```C++
template< class Function, class... Args >
explicit thread(Function&& f, Args&&... args);  (since C++11)
```
上面会构造std::thread并将它和一个线程执行关联。如果未能成功启动新的执行线程，`std::thread`的构造函数就会抛出异常。

通过一个普通函数构造`std::thread`对象
```C++
void do_some_work();
std::thread my_thread(do_some_work);
```

任何可调用类型(callable type)都适用于std::thread, 可以设计一个带有函数调用操作符(function call operator)的类，并将该类实例传递给`std::thread`的构造函数，例如：
```C++
class background_task {
public:
    void operator()() const {
        do_something();
        do_something_else();
    }
};

// 函数对象f作为线程参数, 被复制到属于新线程的存储空间, 在新线程f中调用并执行
// 副本的行为必须与原本的函数对象等效，否则运行结果可能有违预期
background_task f;
std::thread my_thread(f);
```
将函数对象传递给`std::thread`的构造函数时，要注意防范C++’s most vexing parse。假设，传入的是临时变量，而不是具名变量，那么调用构造函数的语法有可能与函数声明相同，此时编译器就会将其解释成函数声明，而不是定义对象。例如，`std::thread my_thread(background_task());`的本意是用`background_task()`生成临时的匿名函数对象，发起新线程。却被解释成函数声明：函数名是`my_thread`，只接收一个参数，返回`std::thread`对象，接收的参数是函数指针，其所指向的函数没有参数传入，返回`background_task`对象。

为临时函数对象命名即可解决问题，可以采用下面的做法
- 多用一对圆括号，来防止被错误解释成函数声明。
- 采用新式的统一初始化语法(uniform initialization syntax, 又名列表初始化)
- 采用lamda表达式(C++11新特性), 属于可调用对象。它准许我们编写局部函数，能捕获某些局部变量，又无须另外传递参数。

例如：
```C++
// 多添加一层()防止background_task被解释成函数声明, 确保其被定义成对象
std::thread my_thread((background_task()));

// 采用初始化语法, 将其声明成变量
std::thread my_thread{background_task()};

// 采用lamda表达式实现可调用对象
std::thread my_thread([]{
    do_something();
    do_something_else();
});
```

启动完线程，我们需要明确等待其结束与之汇合(join)，还是任它独自运行与之分离(detach)。假如等到`std::thread`对象销毁之际还没决定好，那`std::thread`的析构函数将调用`std::terminate()`终止整个程序。我们只需在`std::thread`对象销毁前做出决定——线程本身可能在汇合或分离前就早已结束。如果选择了分离，且分离时新线程还未结束运行，那它将继续运行，甚至在`std::thread`对象销毁很久之后依然运行，它只有最终从线程函数返回时才会结束运行。

如果程序不等待线程结束，那么在线程运行结束前，我们需保证它所访问的外部数据始终正确、有效。因为使用多线程，我们可能会经常面临生存期问题。下面情形会诱发该问题：
- 新线程上的函数持有指针或引用，指向主线程的局部变量；但主线程所运行的函数退出后，新线程却还没结束。例如：
```C++
// 前线程的函数已返回，新线程却仍能访问其局部变量
struct func {
    int& i;
    func(int& i_) : i(i_) {}
    void operator()() {
        for(unsigned j = 0;j < 1000000; ++j) {
            // 隐患:可能访问悬空引用
            do_something(i);
        }
    }
};

void oops() {
    int some_local_state=0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    // 线程分离，不等待新线程结束
    my_thread.detach();
}   // 新线程可能仍在运行，而主线程的函数却已结束
    // do_something依然可能会访问已经被销毁的局部变量some_local_state
    // 从而导致未定义的行为
```
意图在函数中创建线程，并让线程访问函数的局部变量是不可取的, 除非线程肯定会在该函数退出前结束。

上述情形的处理方法通常是：
- 令线程函数完全自含(self-contained)，将数据复制到新线程内部，而不是共享数据。
- 汇合(join)新线程，这样可以确保在主线程的函数退出前，新线程执行完毕。

## 等待线程完成
若需等待线程完成，那么可以在与之关联的`std::thread`实例上，通过调用成员函数`join()`实现。只要调用了`join()`，隶属于该线程的任何存储空间会因此清除，`std::thread`对象就不再关联到已结束的线程。其意义是，对于某个给定的线程，`join()`仅能调用一次；只要`std::thread`对象曾经调用过`join()`，线程就不再可汇合(joinable)，成员函数`joinable()`将返回`false`。
```C++
struct func {
    int& i;
    func(int& i_) : i(i_) {}
    void operator()() {
        for(unsigned j = 0;j < 1000000; ++j) {
            do_something(i);
        }
    }
};

// 现实代码中，原始线程(opps所在线程)可能需要完成自己的工作
// 也有可能发起好几个线程(my_thread所在线程)
// 令它们各自承担实质工作，而原始线程则等待它们全部完成
void oops() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    // 等待线程完成，确保主线程函数退出前新线程执行完毕
    my_thread.join();
}
```

`join()`简单粗暴, 如果需选取更精细的粒度控制线程等待，如查验线程结束与否，或限定只等待一段时间，那我们便得改用其他方式，如条件变量和`future`。

## 在出现异常情况下等待
`std::thread`对象被销毁前，我们需确保已经调用`join()`或`detach()`。通常，若要分离线程，在线程启动后调用`detach()`即可。如果等待线程结束，则需小心地选择执行代码的位置来调用`join()`。原因是，如果线程启动以后有异常抛出，而`join()`尚未执行，则该`join()`调用会被略过。
```C++
struct func;
void f() {
    int some_local_state=0;
    func my_func(some_local_state);
    std::thread t(my_func);
    // 如果这里抛出异常,join调用会被忽略
    do_something_in_current_thread();
    t.join();
}
```

为了防止因抛出异常而导致的应用程序终结, 一般地，如果打算在没发生异常的情况下调用`join()`，那么出现了异常，同样需要调用`join()`，以避免意外的生存期问题。
```C++
struct func;
void f() {
    int some_local_state=0;
    func my_func(some_local_state);
    std::thread t(my_func);
    // 确保无论是正常或异常退出都可以保证线程f()在退出前终结
    try {
        do_something_in_current_thread();
    } catch(...) {
        t.join();
        throw;
    }
    t.join();
}
```
为了达到上面的目标，可以设计一个类利用标准的额RAII手法在析构函数中调用`join()`.
```C++
class thread_guard {
private:
    std::thread& t;
public:
    explicit thread_guard(std::thread& t_) : t(t_) {}
    ~thread_guard() {
        // 先判断线程是否可以汇合之后再进行汇合(很有必要)
        // 因为在任何执行线程(thread of execution)上, join()只能被调用一次
        // 假若线程已经汇合过，那么再次调用join()是错误行为。
        if(t.joinable()) {
            t.join();
        }
    }

    // 复制这类对象或向其赋值均有可能带来问题，因为新对象的生存期也许更长
    // 甚至超过了与之关联的线程
    // 在销毁原对象和新对象时，分别发生两次析构，将重复调用join()
    // 禁用复制构造函数和赋值构造函数,令编译器不生成相关代码
    thread_guard(thread_guard const&) = delete;
    thread_guard& operator=(thread_guard const&) = delete;
};

struct func;
void f() {
    int some_local_state=0;
    func my_func(some_local_state);
    // 构造顺序, 先构造线程t,再构造thread_guard
    std::thread t(my_func);
    thread_guard g(t);
    do_something_in_current_thread();
}   // 析构顺序和构造顺序相反
    // 先析构thread_guard,将先线程t进行join
    // 再析构线程t
```

## 后台运行线程
调用`std::thread`对象的成员函数`detach()`, 会令线程在后台运行，无法与之直接通信。假若线程被分离，就无法等待它完结，也不可能获得与它关联的`std::thread`对象，因而无法汇合该线程。然而分离的线程确实仍在后台运行，其归属权和控制权都转移给C++运行时库(runtime library, 又名运行库)，由此保证，一旦线程退出，与之关联的资源都会被正确回收。
```C++
std::thread t(do_background_work);
// 线程分离,std::thread对象不再关联实际的执行线程, 无法汇合
t.detach();
// 无法汇合
assert(!t.joinable());
```
如果要把`std::thread`对象和线程分离，就必须存在与其关联的执行线程：若没有与其关联的执行线程，便不能在`std::thread`对象上凭空调用`detach()`。这与调用`join()`的前提条件一样，只有当`t.joinable()`返回true时，才能调用`t.detach()`。

UNIX操作系统中，有些进程叫作守护进程(daemon process)，它们在后台运行且没有对外的用户界面；沿袭这一概念，分离出去的线程常常被称为守护线程(daemon thread)。这种线程往往长时间运行。几乎在应用程序的整个生存期内都一直运行，来执行后台任务，如文件系统监控、从对象缓存中清除无用数据项、优化数据结构等。另有一种模式，就是由分离线程执行“启动后即可自主完成”(a fire-and-forget task)的任务；我们还能通过分离线程实现一套机制，用于确认线程完成运行。

考虑一个应用程序，如文字处理软件。为了令它同时编辑多个文件，在用户界面层面和内部层面都有多种实现方式。一种常见的做法是，创建多个独立的顶层窗口，分别与正在编辑的文件逐一对应。
```C++
void edit_document(std::string const& filename) {
    open_document_and_display_gui(filename);
    while(!done_editing()) {
        // 用户输入文件名，那么开启新分离线程进行处理
        user_command cmd = get_user_input();
        if(cmd.type == open_new_document) { // 打开新文件的处理
            std::string const new_name = get_filename_from_user();
            // 创建新线程并给线程函数传递文件名参数new_name
            std::thread t(edit_document, new_name);
            // 将线程分离
            t.detach();
        } else {
            process_user_input(cmd);
        }
    }
}
```
# 向线程函数传递参数
线程具有内部存储空间，参数会按照默认方式先复制到该处，新创建的执行线程才能直接访问它们。然后，这些副本被当成临时变量，以右值形式传给新线程上的函数或可调用对象。即便函数的相关参数按设想应该是引用，上述过程依然会发生。
```C++
void f(int i, std::string const& s);
std::thread t(f, 3, "hello");
```

尽管函数`f()`的第二个参数属于`std::string`类型, 但字符串的字面内容仍以指针char const*的形式传入, 进入新线程的上下文环境以后，才转换为`std::string`类型。如果传递给`f()`函数的第二个参数是指针且指向自动变量(automatic variable),可能会产生重大影响。例如:

```C++
void f(int i,std::string const& s);
void oops(int some_param) {
    // 局部变量位于栈空间，属于自动变量(automatic variable)
    char buffer[1024]; 
    sprintf(buffer, "%i", some_param);
    // buffer以char*的形式传入,进入新线程的上下文才转换为std::string
    // 线程分离，可能将buffer在新线程内转换成std::string之前,oops()函数已经退出
    // 导致访问到已经销毁的字符串数组进而引发未定义的行为
    std::thread t(f, 3, buffer); 
    t.detach();
}
```
问题的根源在于：我们本来希望将指针`buffer`隐式转换成`std::string`对象，再将其用作函数参数，可惜转换未能及时发生，原因是`std::thread`的构造函数原样复制所提供的值，并未令其转换为预期的参数类型。解决方法是，在`buffer`传入`std::thread`的构造函数之前，就把它转化成`std::string`对象。
```C++
void f(int i, std::string const& s);
void not_oops(int some_param) {
    char buffer[1024];
    sprintf(buffer,"%i",some_param);
    // 使用std::string避免悬空指针
    std::thread t(f,3,std::string(buffer));
    t.detach();
}
```

如果线程函数的参数真正想要的是非const引用, 而整个对象却被线程复制到其内部存储空间且,当成move-only类型且以右值形式传递, 会导致编译根本无法通过。解决的办法就是：若需按引用方式传递参数，只要用`std::ref()`函数加以包装即可。例如
```C++
void update_data_for_widget(widget_id w,widget_data& data);
void oops_again(widget_id w) {
    widget_data data;
#if 0
    // 编译出错,因为根据update_data_for_widget的声明, data是引用传入
    // 而线程库的内部代码会将参数的副本(复制到新线程内部储存空间)
    // 当成move-only类型且以右值形式传递
    // update_data_for_widget()预期接受非const引用，不能向它传递右值
    // 解决办法:若需按引用方式传递参数，只要用std::ref()函数加以包装即可
    std::thread t(update_data_for_widget, w, data);
#endif
    // 确保传入的data是指向data变量的引用
    std::thread t(update_data_for_widget, w, std::ref(data));
    display_status();
    t.join();
    process_widget_data(data);
}
```

若要将某个类的成员函数设定为线程函数，应传入一个函数指针，指向该成员函数，还要给出合适的对象指针，作为该函数的第一个参数。
```C++
class X {
public:
    void do_lengthy_work();
};

X my_x;
// 线程实际调用的是my_x.do_lengthy_work()
std::thread t(&X::do_lengthy_work,&my_x);
```

C++11还引入了另一种传递参数的方式：参数只能移动但不能复制，即数据从某个对象转移到另一个对象内部，而原对象则被“搬空”。其中一个例子是`std::unique_ptr`，它为动态分配的对象提供自动化的内存管理。在任何时刻，对于给定的对象，只可能存在唯一一个`std::unique_ptr`实例指向它；若该实例被销毁，所指对象亦随之被删除。通过移动构造(move constructor)函数和移动赋值操作符(move assignment operator), 对象的归属权就得以在多个`std::unique_ptr`实例间转移。函数可以接收这种类型的对象作为参数，也能将它作为返回值，充分发挥其可移动特性，以提升性能。若源对象是临时变量，移动就会自动发生。若源对象是具名变量，则必须通过调用`std::move()`直接请求转移。

借助`std::move()`函数向线程转移动态对象的归属权
```C++
void process_big_object(std::unique_ptr<big_object>);

std::unique_ptr<big_object> p(new big_object);
p->prepare_data(42);
//  std::move(p)会导致big_object对象的归属权发生转移
// 先进入新创建的线程的内部空间, 再转移给process_big_object对象
std::thread t(process_big_object, std::move(p));
```

虽然`std::thread`类的实例并不拥有动态对象(这与`std::unique_ptr`不同)，但它们拥有另一种资源：每份实例都负责管控一个执行线程。因为`std::thread`类的实例能够移动(movable)却不能复制，故线程的归属权可以在其实例之间转移。这保证了，对于任一特定的执行线程，任何时候都只有唯一的`std:::thread`对象与之关联，还准许程序员在其对象之间转移线程归属权。

# 移交线程归属权
假设读者要编写函数，功能是创建线程，并置于后台运行，但该函数本身不等待线程完结，而是将其归属权向上移交给函数的调用者；或相反地，读者想创建线程，遂将其归属权传入某个函数，由它负责等待该线程结束。两种操作都需要转移线程的归属权。这正是`std::thread`支持移动语义的缘由。

线程归属权转移示例
```C++
void some_function();
void some_other_function();
// 启动新线程并关联到t1
std::thread t1(some_function);
// 将t1线程的归属权显式地转移给t2
std::thread t2 = std::move(t1);
// 创建一个临时线程对象并将其归属权转移给t1
t1 = std::thread(some_other_function);

// 按照默认构造的方式构造t3,在创建时，未关联人和执行线程
std::thread t3;
// t2原本关联的线程归属权转移给t3
t3 = std::move(t2);
// 将t3关联的线程归属权转移给t1
t1 = std::move(t3);  // 该赋值操作会终止整个程序
                     // 由于转移前t1与some_other_function的线程关联着
                     // 当前转移，欲将运行some_function()线程的归属权转移到t1
                     // std::terminate()会被调用，终止整个程序
                     // std::thread对象析构前，必须明确等待线程完成(join)还是分离(detach)

// 最终t1与运行some_other_function()的线程关联
// t2没有关联线程
// t3与运行some_function()的线程关联
```
在`std::thread`对象析构前，必须明确：是等待线程完成还是要与之分离。不然，便会导致关联的线程终结。赋值操作也有类似的原则：只要`std::thread`对象正管控着一个线程，不能简单地向它赋新值，否则该线程会因此被遗弃。


`std::thread`支持移动语义的意义：
- **函数可以便捷地向外部转移线程的归属权。**
从函数内部返回std::thread对象示例:
```C++
std::thread f() {
    void some_function();
    return std::thread(some_function);
}

std::thread g() {
    void some_other_function(int);
    std::thread t(some_other_function, 42);
    return t;
}
```

归属权可以转移到函数内部，函数就能够接收std::thread实例作为按右值传递的参数。
```C++
void f(std::thread t);

void g() {
    void some_function();
    f(std::thread(some_function));
    std::thread t(some_function);
    f(std::move(t));
}
```

新类`scoped_thread`: 它的首要目标，是在离开其对象所在的作用域前，确保线程已经完结。
```C++
class scoped_thread {
    std::thread t;
public:
    explicit scoped_thread(std::thread t_) : t(std::move(t_)) {
        if(!t.joinable())
            throw std::logic_error("No thread");
    }

    ~scoped_thread() {
        t.join();
    }
    scoped_thread(scoped_thread const&) = delete;
    scoped_thread& operator=(scoped_thread const&) = delete;
};

struct func;
void f() {
    int some_local_state;
    scoped_thread t{std::thread(func(some_local_state))};
    do_something_in_current_thread();
} // 对象t就会被销毁, 与t关联的线程随即和起始线程join汇合
```

C++20的`std::jthread`和上面的`scoped_thread`很像。

因为`std::thread`支持移动语义，所以只要容器同样知悉移动意图(例如，符合新标准的`std::vector<>`)，就可以装载`std::thread`对象。例如，生成多个线程，并等待它们完成运行。
```C++
void do_work(unsigned id);
void f() {
    std::vector<std::thread> threads;
    for(unsigned i=0;i<20;++i) {
        // 生成线程
        threads.emplace_back(do_work,i);
    }

    // 依次在各线程上调用join()函数，等待所有线程结束运行
    for(auto& entry: threads)
        entry.join();
}
```

# 运行时选择线程数量
C++标准库的`std::thread::hardware_concurrency()`函数的返回值是一指标, 表示程序在各次运行中可真正并发的线程数量。例如，在多核系统上，该值可能就是CPU的核芯数量。如果信息无法获取，该函数则可能返回0。硬件支持的线程数量有限，运行的线程数量不应超出该限度(超出的情况称为线程过饱和, 即oversubscription), 因为线程越多,上下文切换越频繁, 导致性能降低。

并行版的`std::accumulate()`的简单实现
```C++
template<typename Iterator,typename T>
struct accumulate_block {
    void operator()(Iterator first, Iterator last, T& result) {
        result = std::accumulate(first,last,result);
    }
};

template<typename Iterator,typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first,last);
    // 传入区间为空，直接返回初始值(init)
    if(!length) {
        return init;
    }

    unsigned long const min_per_thread = 25;
    // 计算创建线程的最大数量,以防止创建过多线程而造成额外开销
    // 元素总量除以每个线程处理元素的最低限量, 得到线程最大数量,防止发起过多线程
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
    // 硬件线程数量，表示程序在歌词运行中可真正并发的线程数量
    unsigned long const hardware_threads = std::thread::hardware_concurrency();
    // 需要运行的线程数量
    unsigned long const num_threads =
        std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
    // 将目标区间长度除以线程数量, 得出各线程需分担的元素数量
    unsigned long const block_size = length / num_threads;

    // 线程参数确定完成，开始准备装载线程
    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);
    Iterator block_start = first;
    for(unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        // 向线程传入results[i]的引用来获取线程的返回值
        threads[i] = std::thread(
            accumulate_block<Iterator, T>(),
            block_start, block_end, std::ref(results[i]));
        block_start = block_end;
    }
    // 处理最后的一小块(由于计算block_size=length/num_threads可能不能整除, 导致有部分数据未处理)
    accumulate_block<Iterator,T>()(block_start, last, results[num_threads - 1]);

    // 等待所有的线程    
    for(auto& entry : threads) {
        entry.join();
    }

    // 累加中间结果
    return std::accumulate(results.begin(), results.end(), init);
}
```
# 识别线程

线程ID所属型别是`std::thread::id`，函数定义位于头文件`<thread>`内。有两种获取方法：
1. 在与线程关联的`std::thread`对象上调用成员函数`get_id()`，即可得到该线程的ID。如果`std::thread`对象没有关联任何执行线程，调用`get_id()`则会返回一个`std::thread::id`对象，它按默认构造方式生成，表示“线程不存在”。
2. 当前线程的ID可以通过调用`std::this_thread::get_id()`获得。

## 数据操作
C++标准库提供的对线程ID的操作有
- 完整的比较运算，比较运算符对所有不相等的值确立了全序(Total Order)关系。全序关系是关于集合的数学概念，满足3个性质：反对称、传递和完全。这使得它们可以用作关联容器的键值，还可以用于排序，还可以参与任何用途的比较运算。
    ```C++
    operator ==
    
    // folowing removed in C++20
    operator !=
    operator <
    operator <=
    operator >
    operator >=

    // C++20,C++20采用<=>来比较两个参数的大小
    // 返回值采用std::strong_ordering表示,可能取值:
    //  std::strong_ordering::less （lhs less than rhs）
    //  std::strong_ordering::greater（otherwise rhs less than lhs）
    //  std::strong_ordering::equal (otherwise)
    operator<=>
    ```
- 标准库的hash模板计算Hash值，因此`std::thread::id`的值也可以用作新标准的无序关联容器。
    ```C++
    std::hash<std::thread::id>
    ```
- 支持将线程ID输出到输出流。确切的输出内容严格依赖于实现，C++标准只是保证了比较运算中相等的线程ID应产生一样的输出，而不相等的线程ID则产生不同的输出。主要用途是调试除错和记录日志。
    ```C++
    operator<<
    ```
## 常用场景
 1. `std::thread::id`实例常用于识别线程，以判断它是否需要执行某项操作。例如,在运行线程中用线程切分任务，主线程负责发起其他线程且需要完成一些特殊的工作。
    ```C++
    std::thread::id master_thread;
    void some_core_part_of_algorithm() {
        // 保存主线程ID，确保可以识别并完成主线程需要完成的特殊操作
        if(std::this_thread::get_id() == master_thread) {
            // 主线程相对于其他任务进行一些特殊操作
            do_master_thread_work();
        }

        do_common_work();
    }
    ```