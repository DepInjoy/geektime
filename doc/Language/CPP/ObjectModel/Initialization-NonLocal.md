所有具有静态存储期的non-local变量都是在程序启动时，除了延迟初始化(deferred)，它们都在`main`函数开始执行之前初始化

# 静态初始化

静态初始化有两种形式:constant initialization和zero initialization。

- 如果可以，实行constant initialization。
- 否则，non-local静态和thread-local变量进行zero initialization

## constant initialization(常量初始化)

常量初始化是将静态变量的初始值设置为编译时常量。如果静态变量或thread-local变量是constant-initialized，执行常量初始化而非zero initialization，常量初始化在所有其他的初始化之前。

如果变量或临时对象obj满足下面的条件则是constant-initialized：

1. 要么它有一个初始化方法(initializer)
2. 要么它的默认初始化(default-initialization)结果会执行一些初始化操作，并且该初始化完整表达式(full-expression)是一个常量表达式，*除了obj是对象，该完整表达式可以为obj或其subobject调用constexpr constructor，即使这些对象是non-literal class类型。（since C++11）*

```C++
struct S {
    static const int c;
};
const int d = 10 * S::c; // not a constant expression: S::c has no preceding
                         // initializer, this initialization happens after const
const int S::c = 5;      // constant initialization, guaranteed to happen first
 
int main() {
    std::array<int, S::c> a1;	// OK: S::c is a constant expression
	//  std::array<int, d> a2;	// error: d is not a constant expression
}
```

在实践中，常量初始化通常在编译期进行，预计算的object作为程序image的一部分来存储。如果编译器不这样做，编译器仍然需要保证其在任何动态初始化之前进行。

## zero initialization

zero initialization的变量存放在程序image的`.bss`段(segment)中，`.bss`段不占用磁盘空间，且在加载程序时由操作系统清空。

# 动态初始化( Dynamic Initialization)

所有的静态初始化完成后，下面的情况下，non-local变量的动态初始化：

1. Unordered dynamic initialization
2. Partially-ordered dynamic initialization
3. Ordered dynamic initialization

## Early动态初始化(Early Dynamic Initialization)

如果满足下面的条件都满足，编译器允许将动态初始化的变量作为静态初始化的一部分进行初始化：

1. 动态初始化的初始化动作不会改变在其初始化之前的命名空间作用域任何其他对象。
2. 静态初始化产生的值和动态初始化产生的结果值相同。这里的动态初始化指的是这些变量原本不需要静态初始化，那么不提前，也就是不采用静态初始化，依然采用动态初始化。

```C++
inline double fd() { return 1.0; }
extern double d1;


double d2 = d1;   // unspecified:
                  // dynamically initialized to 0.0 if d1 is dynamically initialized, or
                  // dynamically initialized to 1.0 if d1 is statically initialized, or
                  // statically initialized to 0.0 (because that would be its value
                  // if both variables were dynamically initialized)
 
double d1 = fd(); // may be initialized statically or dynamically to 1.0
```



## 延迟动态初始化(Deferred Dynamic Initialization)



# 参考资料

1. [cppreference:initialization](https://en.cppreference.com/w/cpp/language/initialization)
2. [cppreference:constant initialization](https://en.cppreference.com/w/cpp/language/constant_initialization)