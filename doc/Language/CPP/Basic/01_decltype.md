typeof运算符获取对象类型以外，C++标准还提供了一个typeid运算符来获取与目标操作数类型有关的信息。获取的类型信息会包含在一个类型为std::type_info的对象里。

1. `typeid`的返回值是一个左值，且其生命周期一直被扩展到程序生命周期结束。
2. `typeid`返回的`std::type_info`删除了复制构造函数，若想保存`std::type_info`，只能获取其引用或者指针
3. `typeid`的返回值总是忽略类型的 cv 限定符，也就是`typeid(const T)==typeid(T))`

虽然`typeid`可以获取类型信息并判断类型之间的关系，但它并不能像`typeof`那样在编译期就确定对象类型。


C++11标准引入了decltype说明符，使用decltype说明符可以获取对象或者表达式的类型，其语法与typeof类似
```C++
int x1 = 0;
decltype(x1) x2 = 0;
// x2的类型为int
std::cout << typeid(x2).name() << std::endl;

double x3 = 0;
decltype(x1 + x3) x4 = x1 + x3;
// x1+x3的类型为double
std::cout << typeid(x4).name() << std::endl;

// 编译失败，{1, 2}不是表达式
decltype({1, 2}) x5;
```

```C++
struct S1 {
  int x1;
  decltype(x1) x2;
  double x3;
  decltype(x2 + x3) x4;
};

int x1 = 0;
// 在函数的形参列表中使用
decltype(x1) sum(decltype(x1) a1, decltype(a1) a2) {
  return a1 + a2;
}

auto x2 = sum(5, 10);
```

```C++
template<class T>
T sum(T a1, T a2) {
  return a1 + a2;
}

auto x1 = sum(5, 10);
```
如果传递不同类型的实参，则无法编译通过：
```C++
// 编译失败，无法确定T的类型
auto x2 = sum(5, 10.5);
```
`auto`是返回类型的占位符，参数类型分别是`T1`和`T2`，我们利用`decltype`说明符能推断表达式的类型特性，在函数尾部对`auto`的类型进行说明，如此一来，在实例化`sum`函数的时候，编译器能够知道`sum`的返回类型了。
```C++
template<class T1, class T2>
auto sum(T1 a1, T2 a2) -> decltype(a1 + a2) {
  return a1 + a2;
}

auto x4 = sum(5, 10.5);
```

C++14标准已经支持对auto声明的返回类型进行推导,上述代码可以简化为
```C++
template<class T1, class T2>
auto sum(T1 a1, T2 a2) {
  return a1 + a2;
}

auto x5 = sum(5, 10.5);
```

是否从C++14标准以后decltype就没有用武之地了呢？并不是这样的，auto作为返回类型的占位符还存在一些问题，请看下面的例子：
```C++
template<class T>
auto return_ref(T& t) {
  return t;
}

int x1 = 0;
static_assert(
    // auto被推导为值类型
    // 编译错误，返回值不为引用类型
    std::is_reference_v<decltype(return_ref(x1))>
    );
```

```C++
template<class T>
auto return_ref(T& t)->decltype(t) {
  return t;
}

int x1 = 0;
static_assert(
    // 编译成功
    std::is_reference_v<decltype(return_ref(x1))>
    );
```

`decltype(e)`（其中e的类型为T）的推导规则有5条：
1. 如果e是一个未加括号的标识符表达式（结构化绑定除外）或者未加括号的类成员访问，则`decltype(e)`推断出的类型是e的类型T。如果并不存在这样的类型，或者e是一组重载函数，则无法进行推导。
2. 如果e是一个函数调用或者仿函数调用，那么`decltype(e)`推断出的类型是其返回值的类型。
3. 如果e是一个类型为T的左值，则`decltype(e)`是T&。
4. 如果e是一个类型为T的将亡值，则`decltype(e)`是T&&。
5. 除去以上情况，则`decltype(e)`是`T`。

```C++
const int&& foo();
int i;
struct A {
    double x;
};
const A* a = new A();

// decltype(foo())推导类型为const int&&
decltype(foo());
// decltype(i)推导类型为int
decltype(i);
// decltype(a->x)推导类型为double
decltype(a->x);
// decltype((a->x))推导类型为const double& 
decltype((a->x));
```

# cv限定符推导

通常情况下，decltype(e)所推导的类型会同步e的cv限定符，比如：
```C++
const int i = 0;
// 推到类型为const int
decltype(i);
```
当e是未加括号的成员变量时，父对象表达式的cv限定符会被忽略，不能同步到推导结果：
```C++
const int&& foo();
int i;
struct A {
    double x;
};
const A* a = new A();

// decltype(a->x)推导类型为double
decltype(a->x);
```
如果给`a->x`加上括号，情况会不同
```C++
const int&& foo();
int i;
struct A {
    double x;
};
const A* a = new A();

// decltype(a->x)推导类型为const double
decltype((a->x));
```

# decltype(auto)
在C++14标准中出现了decltype和auto两个关键字的结合体：decltype(auto)。它的作用简单来说，就是告诉编译器用decltype的推导表达式规则来推导auto。

```C++
template<class T>
decltype(auto) return_ref(T& t) {
  return t;
}

int x1 = 0;
static_assert(
    // 编译成功
    std::is_reference_v<decltype(return_ref(x1))>
    );
```

与auto一样，在C++17标准中`decltype(auto)`也能作为非类型模板形参的占位符，其推导规则和上面介绍的保持一致
```C++
template<decltype(auto) N>
void f() {
    std::cout << N << std::endl;
}
static const int x  = 11;
static int y  = 11;

int main() {
    // 推导出N的类型为const int
    f<x>();
    // 推导出N的类型为const int&
    f<(x)>();
    // N被推导为int，非常量编译出错
    f<y>();
    // 推导出N的类型为int&,相对于静态对象地址固定
    // 可以顺利编译
    f<(y)>();
}
```