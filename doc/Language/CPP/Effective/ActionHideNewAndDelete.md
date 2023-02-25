当我们采用new创建一个对象的时候，实际上会执行两个动作：其一，通过`operator new`函数分配内存；其二，如果内存分配成功，会调用构造函数。当使用delete来删除一个对象时，也会发生两个动作：其一，调用析构函数；其二，通过`operator delete`函数释放内存。

C++通过`operator new`和 `operator delete`给单一对象分配和释放内存，`Arrays `所用内存由`operator new[]`和`operator delete[]`分配和释放，STL容器所使用的 heap 内存是由容器所拥有的分配器对象(allocator objects)管理，不是被`new`和`delete`直接管理。

# new和delete的使用

直至1993年，C++ 都还要求 operator new必须在无法分配足够内存时返回null。新一代`operator new`则抛出`bad_alloc`异常，但很多C++ 程序是在编译器开始支持新修规范前写出来的。C++ 标准委员提供`nothrow`形式的`operator new`,负责在内存分配失败时返回null。在`delete`时，如果delete的是null指针，则什么都不做，否则释放相应内存。

```C++
// 如果内存分配失败，抛出std::bad_alloc异常
Widget* widget = new Widget();
if (widget) .... // 无法检测到内存分配失败，需要try-catch

// 如果内存分配失败，返回null，即widget=null
Widget* widget = new (std::nothrow)Widget();
if (widget) .... // 检测到内存分配失败，do something
```

但是，**使用`nothrow new` 只能保证`operator new`不抛掷异常,不保证像`new (std::nothrow)Widget`这样额表达式绝不抛出异常。**这是由于`new (std::nothrow) Widget`表达式可能会执行两件事，其一，`nothrow`版的`operator new`被调用，用以分配足够内存给`Widge`t对象。如果分配失败便返回null指针，如果分配成功，会调用`Widget`的构造函数，如果`Widget`的构造函数又通过new来创建对象，可能会抛出异常，如果发生，异常会继续传播。

无论使用抛出异常的new，或是`nothrow-new`，在内存分配失败时都会使用new-handler。

# 理解new-handler行为

当`operator new`无法为对象分配足够内存，会抛出`std::bad_alloc`异常，抛出异常前会先通过`std::get_new_handler()`获取调用客户指定的`std::new_handler`的错误处理函数。

```C++
typedef void (*new_handler)();
std::new_handler set_new_handler( std::new_handler new_p ) noexcept;
std::new_handler get_new_handler() noexcept; // since C++11
```

当`operator new`无法满足内存申请时，它会不断调用new-handler函数，直到找到足够内存或new-handler为空。

假设要申请100000000000大小的内存，内存分配失败，定义new-handler的行为为尝试5次，如果分配失败则卸载。

```C++
#include <iostream>

void OutOfMemory() {
    static int cnt = 0;
    if (cnt++ < 5) {
        std::cerr << cnt << " Unable to satisfy request for memory" << std::endl;
    } else {
        std::set_new_handler(nullptr);
    }
}

int main(int argc, char* argv[]) {
    std::set_new_handler(OutOfMemory);
    try {
        const uint64_t allocSize = 100000000000L;
        int* pBigDataArray = new int[allocSize];
        delete [] pBigDataArray;
    } catch (std::bad_alloc& e) {
        std::cerr << "Alloc failed! " << e.what() << std::endl;
    }
    return 0;
}
```

一个设计良好的new-handler会如下的行为：

- **让更多内存可被使用**。使得`operator new`内的下一次内存分配动作可能成功。实现此策略的一个做法是，程序一开始执行就分配一大块内存，而后当new-handler第一次被调用，将它们释还给程序使用。
- **卸除new-handler**。也就是调用`std::set_new_handler(nullptr)`,此时在`operator new`下一次尝试中会因为内存申请不成功而抛出`std::bad_alloc`的异常。
- **抛出`std::bad_alloc`或派生自`std::bad_alloc`的异常**。该异常不会被`operator new`捕捉，因此会被传播到申请内存的地方。
- **终止程序**。通常调用`std::abort`，`std::exit`或`std::terminate()`.
- **安装另一个new-handler**。如果目前当前的new-handler无法取得更多可用内存，或许它知道另外的new-handler有此能力。当前的new-handler可以通过`set_new_handler`来安装另外的new-handler，即让new-handler修改自己的行为，在下次被调用时，做一些其他的行为。为此，做法之一是令new-handler 修改“会影响new-handler行为”的static数据、`namespace`数据或global数据。

## 自定义类专属new-handler

​	C++ 不支持class专属的 new-handler，可以通过给每个类提供自定的`set_new_handler`和`operator new`行为。

```C++
std::new_handler Widget::set_new_handler(std::new_handler handler) throw() {
    std::new_handler oldNewHandler = currentNewHandler;
    currentNewHandler = handler;
    return oldNewHandler;
}

void* Widget::operator new(std::size_t size) {
    NewHandlerHolder holder(std::set_new_handler(currentNewHandler));
    return ::operator new(size);
}
```

借助`NewHandlerHolder`通过RAII的方式实现构造时保存当前的new-handler，同时在析构时，安装保存的当前的new-handler。

```C++
class NewHandlerHolder {
public:
    explicit NewHandlerHolder(const std::new_handler handler)
        : newHandler_(handler) {}
    ~NewHandlerHolder() {
        // 恢复new-handler
        std::set_new_handler(newHandler_);
    }
private:
    // 记录当前的new-handler
    std::new_handler    newHandler_;

private:
    // 禁止复制和拷贝
    NewHandlerHolder(const NewHandlerHolder&);
    NewHandlerHolder& operator =(const NewHandlerHolder&);
};
```

客户端以下面的方式创建`Widget`

```C++
    // 自定义Widget的new-handler行为
    Widget::set_new_handler(Widget::OutOfMemory);
    Widget* widget = new Widget();
	
	int* arrayInt = new int[8];
	....
```

[为Widget类自定义类专属new-handler实现源码](https://github.com/DepInjoy/geektime/blob/main/ProgramLanguage/CPP/MemManager/Specific_NewHandler.cpp)

## 通用类自定义专属new-handler

借助RAII和CRTP(Curously Recurring template pattern)模板继承实现一个通用的类自定义的new-handler。

```C++
template<typename T>
class NewHandlerSupport {
public:
    virtual ~NewHandlerSupport() {
        // 恢复new-handler
        std::set_new_handler(curNewHandler_);
    }
    static std::new_handler set_new_handler(std::new_handler handler) throw();
    void* operator new(std::size_t size);

private:
    // 记录当前的new-handler
    static std::new_handler    curNewHandler_;
};

template<typename T>
std::new_handler NewHandlerSupport<T>::curNewHandler_ = nullptr;

template<typename T>
std::new_handler NewHandlerSupport<T>::set_new_handler(
        std::new_handler handler) throw() {
    std::new_handler oldHanler = curNewHandler_;
    curNewHandler_ = handler;
    return oldHanler;
}

template<typename T>
void* NewHandlerSupport<T>::operator new(std::size_t size) {
    NewHandlerSupport<T>::set_new_handler(std::set_new_handler(curNewHandler_));
    return ::operator new(size);
}
```

自定义的`Widget`继承`NewHandlerSupport`

```C++
class Widget : public NewHandlerSupport<Widget>
```

[通用类自定义专属new-handler实现源码](https://github.com/DepInjoy/geektime/blob/main/ProgramLanguage/CPP/MemManager/Template_NewHandler.cpp)

# 参考资料

- ### Effective C++：改善程序与设计的55个具体做法