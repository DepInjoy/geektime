# 静态变量初始化

常量初始化(Constant initialization)将静态变量的初始值设置为编译时常量。

```C++

```



## 静态变量初始化顺序失败

> The *static initialization order fiasco* refers to the ambiguity in the order that objects with static storage duration in different translation units [are initialized](https://en.cppreference.com/w/cpp/language/initialization#Non-local_variables) in. If an object in one translation unit relies on an object in another translation unit already being initialized, a crash can occur if the compiler decides to initialize them in the wrong order. For example, the order in which .cpp files are specified on the command line may alter this order. The Construct on First Use Idiom can be used to avoid the static initialization order fiasco and ensure that all objects are initialized in the correct order.
>
> Within a single translation unit, the fiasco does not apply because the objects are initialized from top to bottom.
>
>  						-- 来自:[cppreference:Static Initialization Order Fiasco](https://en.cppreference.com/w/cpp/language/siof#:~:text=The%20static%20initialization%20order%20fiasco,translation%20units%20are%20initialized%20in.)

大意是：静态初始化顺序问题是指在不同翻译单元中具有静态存储期的对象初始化顺序存在歧义。如果一个翻译单元中的对象依赖于另一个已经初始化的翻译单元中的对象，如果编译器错误的顺序初始化它们，就有可能发生崩溃。例如，在命令行中指定的`.cpp`文件的顺序可能会改变这个顺序。第一次使用时构造(Construct on First Use)惯用法可以避免静态初始化顺序失败，并确保所有对象都按照正确的顺序初始化。

在单个翻译单元中，由于对象是从上到下初始化的，所以不会出现这种失败。

