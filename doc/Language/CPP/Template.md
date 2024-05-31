模板
模板属性(Template Attributes),用于控制模板实例化的行为
模板变量(Template Variables), C++17 中引入的一个特性，用于创建通用的变量。

模板元编程

如果在类中有一个模板成员函数，并且需要在该成员函数中调用其他模板函数或模板类的成员，可以使用this->template显式指定模板成员函数的名称
```C++
template<typename T>
class MyClass {
public:
    template<typename U>
    void myTemplateFunction(U val) {
        // 调用其他模板函数或模板类的成员
        this->template myOtherTemplateFunction<U>(val);
    }

    template<typename U>
    void myOtherTemplateFunction(U val) {
        // 实现
    }
};
```

# 函数模板
函数模板提供了一种函数行为，该函数行为可以用多种不同的类型进行调用；也就是说，函数模板代表一个函数家族。它看起来和普通的函数很相似，唯一的区别就是有些函数元素是未确定的：这些元素将在使用时被参数化。

函数模板的格式:
```C++
template <typename 类型参数1, typename 类型参数2, ...>
返回值类型  模板名(形参表) {
    函数体
}
```

## 使用方式
**1. 将模板函数的实现也放在头文件中。**
 
该方式可以按需生成需要的实例。由于编译器即知道函数模板的定义有知道具体的函数模板的类型参数，因此可以按需生成需要的实例，缺点是必须将实现细节暴露给用户。

```C++
// MaxInc.h
// T为模板参数, t1和t2为调用参数
template<typename T>
T const& Max(const T& t1, const T& t2) {
    return (t1 < t2) ? t2 : t1;
}

// FunctionTemplate.cpp
#include <iostream>
#include "MaxInc.h"

int main(int argc, char* argv[]) {
    int i1 = 42, i2 = 8;
    std::cout << "max(i1, i2):  " << Max(i1, i2) << std::endl;

    double f1 = 3.4, f2 = -6.7;
    std::cout << "max(f1, f2):  " << Max(f1, f2) <<std::endl;

    std::string s1 = "mathematics", s2 = "math";
    std::cout << "max(s1, s2):  " << Max(s1, s2) <<std::endl;
    return 0;
}
```
**2. 采取声明和实现分离的方案。**

在CPP文件中添加特定版本的实例化，该方案能可以隐藏实现细节,也可以限制只实例化特定的版本,缺点是只能使用特定实例化的几个版本，根据具体的调用情况按需生成。
```C++
// Max.h
template<typename T>
T const& Max(const T& t1, const T& t2);

// Max.cpp
#include "Max.h"
#include <string>

template<typename T>
T const& Max(const T& t1, const T& t2) {
    return (t1 < t2) ? t2 : t1;
}

// 函数模板的实例化(Function Template Instantiation)
template int const& Max(const int& t1, const int& t2);
template double const& Max(const double& t1, const double& t2);
template std::string const& Max(const std::string& t1, const std::string& t2);

// FunctionTemplate.cpp
#include <iostream>
#include "Max.h"

int main(int argc, char* argv[]) {
    int i1 = 42, i2 = 8;
    std::cout << "max(i1, i2):  " << Max(i1, i2) << std::endl;

    double f1 = 3.4, f2 = -6.7;
    std::cout << "max(f1, f2):  " << Max(f1, f2) <<std::endl;

    std::string s1 = "mathematics", s2 = "math";
    std::cout << "max(s1, s2):  " << Max(s1, s2) <<std::endl;
    return 0;
}
```

[两种方案实现代码通过DECLARE_IMPL_SEPARATION进行条件编译](https://github.com/DepInjoy/geektime/blob/main/ProgramLanguage/CPP/Template/Basic/FunctionTemplate.cpp)

通常而言，并不是把模板编译成一个可以处理任何类型的单一实体；而是对于实例化模板参数的每种类型，都从模板产生出一个不同的实体。例如`max(i1,i2)`的调用，调用的是如下代码的语义
```C++
int const& max(const int& t1, const int& t2) {
    return (t1 < t2) ? t2 : t1;
}
```
C++模板是通过实例化(instantiation)来实现多态(polymorphism)的，用具体类型代替模板参数的过程叫做实例化(instantiation)，它产生了一个模板的实例。只要使用函数模板，编译器会自动地引发这样一个实例化过程。

模板被编译两次，分别发生在
1. 实例化之前，先检查模板代码本身，查看语法是否正确；在这里会发现错误的语法，如遗漏分号等。
2. 在实例化期间，检查模板代码，查看是否所有的调用都有效。如该实例化类型不支持某些函数调用等。

因此，当使用函数模板，并且引发模板实例化的时候，编译器需要查看模板的定义。这不同于普通函数中编译和链接之间的区别，因为对于普通函数而言，只要有该函数的声明（即不需要定义），就可以顺利通过编译。

## 实参演绎(deduction)
当我们为某些实参调用一个诸如`max()`的模板时，模板参数可以由我们所传递的实参来决定。如果我们传递了两个int给参数类型`T const&`，那么C++编译器能够得出结论：`T`必须是`int`。注意，不允许进行自动类型转换；每个T都必须正确地匹配。
```C++
max(4, 7);   // OK
max(4, 3.7); // Error
```
有三种方式处理该问题：
有3种方法可以用来处理上面这个错误：
1. 对实参进行强制类型转换，使它们可以互相匹配
```C++
  max(4, static_cast<int>(3.7));
```
2. 显式指定T的类型
```C++
max<double>(4, 3.7);
```
3. 指定两个参数可以具有不同的类型。
```C++
template<typename T1, typename T2>
T1 max(const T1& t1, const T2& t2) {
    return (t1 < t2) ? t2 : t1;
}
```
针对该实现存在一些问题，主要问题在于：我们必须声明返回类型，将`T1`作为函数的返回类型，那么如果第二个参数被返回，就会需要创建一个局部的临时对象，导致无法使用引用返回结果，所以它的返回值类型需要是`T1`,不可以是`T1 const&`. 

## 模板参数
因为调用参数的类型构造自模板参数，所以模板参数和调用参数通常是相关的,这被称为：函数模板的**实参演绎**，它使得可以像调用普通函数那样调用函数模板。模板实参演绎并不适合返回类型，因为RT不会出现在函数调用参数的类型里面。因此，函数调用并不能演绎出RT，必须显式地指定模板实参列表。

```C++
template<typename RT, typename T1, typename T2>
RT max(const T1& t1, const T2& t2) {
    return (t1 < t2) ? t2 : t1;
}

// 显式指定所有模板参数
std::cout << max<double, double, double>(4, 3.7) << std::endl;

// T1和T2实参演绎为int和double
std::cout << max<double>(4, 3.7) << std::endl;
```
## 重载函数模板
```C++
#include <iostream>

int const& max(const int& t1, const int& t2) {
    return (t1 < t2) ? t2 : t1;
}

template<typename T>
T const& max(const T& t1, const T& t2) {
    return (t1 < t2) ? t2 : t1;
}

template <typename T>
T const& max (T const& a, T const& b, T const& c) {
    return ::max (::max(a,b), c);
}

int main(int argc, char* argv[]) {
    ::max(7, 42, 68);       // 调用具有3个参数的模板
    ::max(7.0, 42.0);       // 调用max<double> (通过实参演绎)
    ::max('a', 'b');        // 调用max<char> (通过实参演绎)
    ::max(7, 42);           // 调用int重载的非模板函数
    ::max<>(7, 42);         // 调用 max<int> (通过实参演绎)
    ::max<double>(7, 42);   //调用max<double> (没有实参演绎)
    ::max('a', 42.7);       //调用int重载的非模板函数
    return 0;
}
```
1. 对于非模板函数和同名的函数模板，如果其他条件都是相同的话，那么在调用的时候，重载解析过程通常会调用非模板函数，而不会从该模板产生出一个实例，例如
```C++
::max(7, 42);           // 调用int重载的非模板函数
```

2. 如果模板可以产生一个具有更好匹配的函数，那么将选择模板
```C++
::max(7.0, 42.0);       // 调用max<double> (通过实参演绎)
::max('a', 'b');        // 调用max<char> (通过实参演绎)
```

3. 显式地指定一个空的模板实参列表，告诉编译器：只有模板才能来匹配这个调用，而且所有的模板参数都应该根据调用实参演绎出来
```C++
::max<>(7, 42);         // 调用 max<int> (通过实参演绎)
```
4. 模板是不允许自动类型转化的；但普通函数可以进行自动类型转换
```C++
::max('a', 42.7);       //调用int重载的非模板函数
```

复杂的重载会导致错误

```C++
template<typename T>
T max(const T& t1, const T& t2) {
    return (t1 < t2) ? t2 : t1;
}

template <typename T>
T const& max (T const& a, T const& b, T const& c) {
    return ::max(::max(a,b), c);
}
```
对于`::max(::max(a,b), c)`的调用，对于C-strings而言，`max(a,b)`产生了一个新的临时局部值，该值有可能会被外面的max函数以传引用的方式返回，而这将导致传回无效的引用。

```C++
template<typename T>
T const& max(const T& t1, const T& t2) {
    return (t1 < t2) ? t2 : t1;
}

template <typename T>
T const& max (T const& a, T const& b, T const& c) {
    return ::max(::max(a,b), c); // 无法看到下面的int的max版本
}

int const& max(const int& t1, const int& t2) {
    return (t1 < t2) ? t2 : t1;
}
```
上述实现如果发生`max(5, 6, 7)`的函数调用，执行的模板函数实例化出来的实体。

因此，需要注意**函数的所有重载版本的声明都应该位于该函数被调用的位置之前。**

# 参考资料
- C++ Templates(中文版)书籍
- [原理：C++为什么一般把模板实现放入头文件](https://www.cnblogs.com/zpcdbky/p/16329886.html)