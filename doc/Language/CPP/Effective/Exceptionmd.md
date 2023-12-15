# 条款9:利用destructors避免泄漏资源

假设你在为一个“小动物收养保护中心”编写一个软件，收养中心每天都会生成一个文件，由它安排当天的收养个案，书写一个程序来读取这些文件，为每个收养个案做适当的处理。

合理的想法是定义一个抽象基类ALA("Adorable Little Animal"),再派生出针对小狗和小猫的具体类。其中有个虚函数 `processAdoption`，负责“因动物种类而异”的必要处理动作。

```C++
class ALA {
public:
    // 基类的析构函数为虚函数确保派生类析构会调用基类析构
    virtual ~ALA() {
        std::cout << "~ALA" << std::endl;
    }
    virtual void processAdoption() = 0;
};

class Puppy : public ALA {
public:
    ~Puppy() {
        std::cout << "~Puppy" << std::endl;
    }
    void processAdoption() override {
        std::cout << "Puppy::processAdoption" << std::endl;
        // throw std::runtime_error("just for test");
    }
};
```

假设通过new在堆上申请一个对象处理收养事项

```C++
    {
        ALA* puppy(new Puppy);
        puppy->processAdoption();
        delete puppy;
    }
```

如果`processAdoption`抛出异常，那么便需要对异常处理

```C++
    {
        ALA* puppy(new Puppy);
        try {
            puppy->processAdoption();
        } catch (std::exception& e) {
            std::cerr << e.what() << '\n';
            delete puppy;	// 捕获异常调用delete来避免资源泄漏
        }
        delete puppy;
    }
```

这样导致无论程序运行时正常还是异常都需要delete来释放资源，可以尝试将其其中于一处来做这件事情？因为不论函数如何结束，局部对象总是会在函数结束时被析构。于是，我们真正感兴趣的是，如何把 delete 动作从转移到函数内某个局部对象的destructor 内。解决办法就是，以一个“类似指针的对象”取代指针 ，而当这个类似指针的对象被（自动）销毁，令其 destructor 调用 delete。C++ 标准程序库提供了` std::unique_ptr`(`std::auto_ptr`在C++11中已经被废除)。

上述实现可以采取`std::unique_ptr`来取代裸指针，并确保即使在异常抛出的情况下，堆上的对象依然可以被删除。

```C++
    try {
        std::cout << "1) 采用unique_ptr包装裸指针,避免资源泄漏,同时支持多态" << std::endl;
        std::unique_ptr<ALA> puppy(new Puppy);
        puppy->processAdoption();
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
```



隐藏在`std::unique_ptr`背后的观念——**以一个对象存放“必须自动释放的资源”，并依赖该对象的destructor 释放**。考虑图形界面（GUI）应用软件中的某个函数，它必须产生一个窗口以显示某些信息

```C++
// 可能会在抛出exception之后发生资源泄漏问题
void displayInfo(const Information& info) {
    WINDOW_HANDLE w(createWindow()); // 产生窗口，取得资源
    // 显示信息
    destroyWindow(w); // 释放资源
}
```

如果在显示信息过程中产生异常，那么就可能会产生资源泄漏。解决方法和先前类似，设计一个类，令其在构造函数和析构函数内分别取得资源和释放资源。

```C++
class WINDOW_HANDLE {
public:
    void displayInfo() {
        std::cout << "Display Info" << std::endl;
        // throw std::runtime_error("displayInfo exception just for test");
    }
};

WINDOW_HANDLE createWindow() {
    WINDOW_HANDLE wh;
    std::cout << "createWindow" << std::endl;
    return wh;
}

void destroyWindow(const WINDOW_HANDLE& handle) {
    std::cout << "destroyWindow" << std::endl;
}

class WindowHandle {
public:
    explicit WindowHandle(WINDOW_HANDLE handle) : w(handle) {}
    ~WindowHandle() { destroyWindow(w); }
    // 隐式转换,将WindowHandle转换为WINDOW_HANDLE
    operator WINDOW_HANDLE() const { return w; }

private:
    WINDOW_HANDLE w;
    WindowHandle(const WindowHandle&);
    WindowHandle& operator=(const WindowHandle&);
};
```

假设在`displayInfo`处理中抛出异常，也可以确保调用`destroyWindow`来释放资源。

```C++
    try {
        std::cout << "\n2) 将WINDOW_HANDLE封装在WindowHandle对象内来避免资源泄漏" << std::endl;
        WindowHandle wh2(createWindow());
        static_cast<WINDOW_HANDLE>(wh2).displayInfo();
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
```

在`WindowHandle`提供了`operator WINDOW_HANDLE() const`隐式转换来实现可以在以`WINDOW_HANDLE`入参的函数中支持以`WindowHandle`来入参，例如

```C++
// 展示隐式转换带来的方便之处
// 由于WindowHandle支持隐式转换为WINDOW_HANDLE, 函数入参可以为WindowHandle
void ConvenientWindowHandlerDisplay(WINDOW_HANDLE winHandle) {
    winHandle.displayInfo();
}

WindowHandle wh2(createWindow());
ConvenientWindowHandlerDisplay(wh2);
```

## `unique_ptr`支持自定义deleter

```C++
void CloseFile(std::FILE* fd) {
    std::cout << "close file" << std::endl;
    std::fclose(fd);
}

int main(int argc, char* argv[]) {
    try {
        {
            std::cout << "\n3) Custom deleter demo" << std::endl;
            std::ofstream("demo.txt") << 'x';  // prepare the file to read
            using UniqueFilePtrType = std::unique_ptr<std::FILE, decltype(&CloseFile)>;
            UniqueFilePtrType ufd(std::fopen("demo.txt", "r"), &CloseFile);
            if (ufd) {
                std::cout << char(std::fgetc(ufd.get())) << std::endl;;
            }
        }

        {
            std::cout << "\n4) Custom lambda-expression deleter and exception safety demo"
                	<< std::endl;
            std::unique_ptr<Puppy, void(*)(Puppy*)> p(new Puppy, [](Puppy* ptr) {
                std::cout << "destroying from a custom deleter...\n";
                delete ptr;
            });
            // throw std::runtime_error("custom lambda-expression deleter : just for test");
        }
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}

```

上述实现[完整实现代码](https://github.com/DepInjoy/geektime/blob/main/ProgramLanguage/CPP/Exception/09_DestructorAvoidResourceLeak.cpp)

## 参考资料

1. [cppreference:unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr)
2. 《More Effective C++:35个改善编程与设计的有效方法(中文版)》

# 条款12:了解“抛出一个 exception”与“传递一个参数”或“调用一个虚函数”间差异
函数参数和exceptions的传递方式有3种：by value， by reference，by pointer。**当调用一个函数，控制权最终会回到调用端（除非函数失败以至于无法返回），但是当抛出 exception，控制权不会再回到抛出端。**

```C++
        Widget localWidget;
        throw localWidget;  // localWidget作为异常被抛出会发生复制，将副本抛出
```

 **C++ 特别声明，一个对象被抛出作为 exception时，总是会发生复制(copy)。**导致抛出异常通常比传递参数要慢。不论被捕捉的exception 是以by value 或 by reference方式传递，都会发生`localWidget`的复制行为，而交到 catch 子句上的是副本。即使此exception 以by reference 方式被捕捉，catch 端还是不可能修改`localWidget`，只能修改 `localWidget`的副本。下面的代码对于上述描述可以进行更直观的展示:

```C++
class Widget {
public:
    Widget() = default;
    Widget(const Widget& widget) {
        std::cout << "Copy Constructtor" << std::endl;
    }

    void ShowWidgetInfo() {
        std::cout << this << std::endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        Widget localWidget;
        localWidget.ShowWidgetInfo();
        throw localWidget;  // localWidget作为异常被抛出会发生复制
    }  // 将副本抛出
    catch (const Widget& e) {  // 捕获localWidget的副本，可以看到两个指针不同
        const_cast<Widget&>(e).ShowWidgetInfo();
    }
    return 0;
}
```

**当对象当作异常被复制，复制行为是由对象的复制构造( copy constructor)执行，copy constructor 相应于该对象的“静态类型”而非“动态类型”。**代码对于该描述的解释如下：

```C++
class SpecialWidget: public Widget {
public:
    SpecialWidget() = default;
    SpecialWidget(const SpecialWidget& widget) {
        std::cout << "SpecialWidget : Copy Constructtor" << std::endl;
    }
};

try {
    SpecialWidget specialWidget;
    Widget& rw = specialWidget;
    throw rw;  // rw的静态类型为Widget，执行Widget的复制构造函数创建副本并抛出Widget类型的异常
} catch (const SpecialWidget& e) {
    std::cout << "SpecialWidget Exception" << std::endl;
} catch (const Widget& e) {  // 可以捕获Widget和SpecialWidget异常
    std::cout << "Widget Exception" << std::endl;
}
```

由于抛出的异常对象是对象副本，这导致如何 catch 语句块内传播 exceptions存在差异,假设在`try`语句块中抛出的是`SpecialWidget`。

```C++
try {
    SpecialWidget specialWidget;
    throw specialWidget;
}
```

下面两个实现继续传播异常

```C++
catch(Widget& e) {
    throw;    // 抛出当前异常，抛出是SpecialWidget类型的异常
}

catch(Widget& e) {
    throw e;  // 抛出Widget类型的异常
}
```

函数调用过程中将一个临时对象传递给一个 non-const reference 参数是不允许的，但对exceptions则属合法。

```C++
catch(Widget e)	 // 以by value的方式捕获异常，会发生两次复制构造(抛出对象产生副本+将临时对象复制到w)
catch(Widget& e)  // 以by reference方式捕获异常
catch(const Widget& e)  // 以by reference方式捕获异常
```

throw by pointer 事实上相当于 pass by pointer，两者都传递指针副本。必须特别注意的是，千万不要抛出一个指向局部对象的指针，因为该局部对象会在 exception 传离作用域时被销毁。

“自变量传递”与“exception 传播”两动作有着互异的做法：

- 其中一个不同是对象从“调用端或抛出端”转移到“参数或 catch子句”时的做法。如上述描述
- 第二个不同是“调用者或抛出者”和“被调用者或捕捉者”之间的类型匹配(type match)规则。

exceptions 与 catch 子句匹配的过程，仅有两种转换可以发生

- 一种是“继承架构中的类转换(inheritance-based conversions)”。一个针对 base class exceptions而编写的catch 子句，可以处理类型为 derived class的exceptions。

    ```C++
       try {
          ...
        } catch (logic_error& ex) {
            // 此语句块将捕捉所有的logic_error exceptions,甚至包括其 derived types
        } catch (invalid_argument& ex) {
            // 此语句块绝不会被执行，因为所有的invalid_argument是logic_error的派生类
            // 都会被上面的catch子句捕获
        } 
    ```

    当你调用一个虚函数，被调用的函数是“调用者（某个对象）的动态类型”中的函数。可以说，虚函数采用所谓的“best fit”（最佳匹配）策略，而exception 处理机制遵循所谓的“first fit”（最先吻合）策略。

    ```C++
       try {
          ...
        } catch (invalid_argument& ex) {
            // 处理invalid_argument异常
        } catch (logic_error& ex) {
            // 处理其他的logic_error异常
        } 
    ```

- 第二个允许发生的转换是从一个“有型指针”转为“无型指针”，所以一个针对`const void*` 指针而设计的catch子句，可捕捉任何指针类型的exception。

上述实现[完整实现代码](https://github.com/DepInjoy/geektime/blob/main/ProgramLanguage/CPP/Exception/12_Diif_ThrowAndFunction.cpp)

## 参考资料

1. 《More Effective C++:35个改善编程与设计的有效方法(中文版)》

# 条款13:以by reference方式捕捉exception
# 条款14: 明智运用exception specification

# 条款15:了解异常处理（exception handling）的成本

**编译过程中如果没有加上对 exceptions的支持，程序通常比较小，执行时也比较快。如果编译过程中加上对 exceptions的支持，程序就比较大，执行时也比较慢**。如果你的程序没有任何一处使用 try, throw 或catch，而且你连接的程序库没有一个有用到 try, throw 或 catch ，可以在编译过程中放弃支持exception，并因而免除了大小和速度的成本。目前，程序对 exceptions的运用普及度愈来愈高，但是如果你决定不使用 exceptions，并让编译器知道，编译器可以适度完成某种性能优化。

Exception 处理机制带来的第二种成本来自 try 语句块。**粗略估计，如果使用 try 语句块，代码大约整体膨胀 5%～10%，执行速度亦大约下降这个数。**面对 exception specifications，编译器产出的代码倾向于类似面对 try 语句块的行为，所以一个exception specification 通常会招致与 try 语句块相同的成本

**和正常的函数返回动作比较，由于抛出exception而导致的函数返回，其速度可能比正常情况下慢3个数量级。**

为了让 exception的相关成本最小化，只要能够不支持 exceptions，编译器便不支持；请将你对 try 语句块和exception specifications的使用限制于非用不可的地点，并且在真正异常情况下才抛出 exceptions。

## terminate

[cppreference:std::terminate](https://en.cppreference.com/w/cpp/error/terminate)


# rethrow数据竞争
> Concurrently calling rethrow_exception on exception_ptr objects that refer to the same exception is safe.
>
> Note though that some implementations may not perform a copy of the pointed object on entering the catch exception handling block, and concurrently accessing the rethrown exception object in this case may introduce a data race.
>
>  -- 来自[rethrow_exception](https://cplusplus.com/reference/exception/rethrow_exception/)

