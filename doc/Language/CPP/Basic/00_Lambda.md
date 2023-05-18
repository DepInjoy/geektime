从C++11开始支持lambda表达式，其语法：

```C++
[ captures ] ( params ) specifiers exception -> ret { body }
```

例如:

```C++
#include <iostream>
void main() {
    int x = 5;
    // 按值捕获x,函数入参y，返回值为int,实现计算并返回x*y
    auto foo = [x](int y)->int { return x * y; /*函数实现主体*/};
}
```



`[captures]`： 捕获列表，它可以捕获当前函数作用域的零个或多个变量，变量之间用逗号分隔。捕获列表的捕获方式有两种：按值捕获和引用捕获。Lambda表达式的列表捕获方法除了指定捕获变量之外，还有：

- `[this]`：捕获`this`指针，让我们使用`this`类型的成员变量和函数。
- `[=]`：捕获Lambda表达式定义作用域的全部变量的值，包括`this`。
- `[&]`：捕获Lambda表达式定义作用域的全部变量的引用，包括`this`。

`(params)`：可选参数列表，和普通函数的参数列表一样，在不需要参数的时候可以忽略参数列表。

`specifiers`：可选限定符，C++11中可以用`mutable`，它允许我们在lambda表达式函数体内改变按值捕获的变量，或者调用非`const`的成员函数。

`exception` ：可选异常说明符，可以使用`noexcept`来指明lambda是否会抛出异常。

`ret`：可选返回值类型。不同于普通函数，lambda表达式使用返回类型后置的语法来表示返回类型，如果没有返回值(void类型)，可以忽略包括->在内的整个部分。也可以在有返回值的情况下不指定返回类型，编译器会推导出一个返回类型。

`body`：Lambda表达式的函数体。

# 捕获列表

捕获列表中的变量存在于两个作用域：

1. Lambda表达式定义的函数作用域，为了捕获变量
2. Lambda表达式函数体的作用域，为了使用变量。

标准还规定能捕获的变量必须是一个自动存储类型，也就是就是非静态的局部变量。如果在Lambda表达式需要使用全局变量或者静态局部变量，可以直接使用。如果将一个lambda表达式定义在全局作用域，那么lambda表达式的捕获列表必须为空

```C++
int x = 1;
// lambda表达式定义在全局作用域,捕获列表必须为空
auto getX() -> int { return x; }
int main() {
    int y = 2;
    static int z = 3;

    // 由于x和z不是自动存储类型的变量不可以捕获，直接使用
    auto foo = [y] { return x + y + z; };
    std::cout << "foo = " << foo() << std::endl;
    std::cout << "x = " << getX() << std::endl;
    return 0;
}
```



捕获列表的捕获方式分为

1. 捕获值，其中捕获值在`[]`中直接写入变量名，如果有多个变量，则用逗号分隔，此时无法更改变量的值(Lambda表达式的一个特性：捕获的变量默认为常量，或者说lambda是一个常量函数(类似于常量成员函数))。
2. 捕获引用，只需要在捕获变量的前面添加`&`,此时捕获的变量是一个引用，可以改变变量的值。

使用`mutable`说明符可以移除lambda表达式的常量性，实现可以在lambda表达式的函数体中修改捕获值的变量，但和捕获引用是不同的。lambda表达式捕获值时，表达式内实际获得的是捕获变量的复制，我们可以任意地修改内部捕获变量，但不会影响外部变量，但是却能影响下次调用lambda表达式时变量的值。而捕获引用则不同，在lambda表达式内修改捕获引用的变量，对应的外部变量也会被修改。

```C++
{
    int x = 5, y = 8;
    // mutable可以移除lambda的常量属性,可以在lamada表达式内部改变按值捕获变量的值
    // 这次改变不影响外部变量，但是会影响下一次调用lamdda表达式的值
    // 按引用捕获可以在表达式内部修改变量的值且同时修改对应外部变量的值
    auto foo = [x, &y]() mutable {
        x += 1;
        y += 2;
        std::cout << "lambda x = " << x << ", y = " << y << std::endl;
        return x * y;
    };
    foo();
    std::cout << "call1  x = " << x << ", y = " << y << std::endl;
    std::cout << std::endl;

    foo();
    std::cout << "call2  x = " << x << ", y = " << y << std::endl;
}
```

上面的执行结果

```
lambda x = 6, y = 10
call1  x = 5, y = 10

lambda x = 7, y = 12
call2  x = 5, y = 12
```



捕获值的变量在lambda表达式定义的时候已经固定，无论函数在lambda表达式定义后如何修改外部变量的值，lambda表达式捕获的值都不会变化。

```C++
{
    int x = 5, y = 8;
    // 按值捕获的变量在lambda表达式定义时固定,外部后续修改,捕获的值不变化
    auto foo = [x, &y]() mutable {
        x += 1;
        y += 2;
        std::cout << "lambda x = " << x << ", y = " << y << std::endl;
        return x * y;
    };
    x = 9, y = 20;
    foo();
}
```

在调用`foo`之前分别修改了`x`和`y`的值，但是捕获值的变量`x`依然延续着lambda定义时的值，而在捕获引用的变量`y`被重新赋值以后，lambda表达式捕获的变量`y`的值也发生了变化，上面执行结果:

```
lambda x = 6, y = 22
```



lambda表达式的捕获列表除了指定捕获变量之外还有3种特殊的捕获方法。

1. `[this]` ： 捕获`this`指针，可以使用`this`类型的成员变量和函数。
2. `[=]` ：捕获lambda表达式定义作用域的全部变量的值，包括this。
3. `[&]`：捕获lambda表达式定义作用域的全部变量的引用，包括this。

```C++
{
    int x = 5, y = 8;
    // 捕获Lambda作用域内的所有变量的值
    auto foo = [=] { return x * y; };
    std::cout << "foo = " << foo() << std::endl;
    
    // 捕获Lambda作用域的全部变量的引用
    auto foo2 = [&] { ++x, ++y; };
    foo2();
    std::cout << "x = " << x << ", y = " << y << std::endl;
}
```

.

# 实现原理

lambda表达式与函数对象(仿函数)非常相似，从函数对象开始深入探讨lambda表达式的实现原理。

```C++
#include <iostream>

class Bar {
public:
    Bar(int x, int y) : x_(x), y_(y) {}
    int operator () () {
        return x_ * y_;
    }
private:
    int x_;
    int y_;
};

int main() {
    int x = 5, y = 8;
    auto foo = [x, y] { return x * y; };
    Bar bar(x, y);
    std::cout << "foo() = " << foo() << std::endl; // 40
    std::cout << "bar() = " << bar() << std::endl; // 40
}
```

在上面的代码中，`foo`是一个lambda表达式，而`bar`是一个函数对象。它们都能在初始化的时候获取`main`函数中变量`x`和`y`的值，并在调用之后返回相同的结果。这两者比较明显的区别如下。

1. 使用lambda表达式不需要我们去显式定义一个类，在快速实现功能上有较大的优势。
2. 使用函数对象可以在初始化的时候有更加丰富的操作，例如`Bar bar(x+y, x * y)`，而这个操作在C++11标准的lambda表达式中是不允许的。另外，在`Bar`初始化对象的时候使用全局或者静态局部变量也是没有问题的。

lambda表达式的优势在于书写简单方便且易于维护，而函数对象的优势在于使用更加灵活不受限制，但总的来说它们非常相似。这正是lambda的实现原理。

lambda表达式在编译期会由编译器自动生成一个闭包类，在运行时由这个闭包类产生一个对象，我们称它为闭包。在C++中，所谓的闭包可以简单地理解为一个匿名且可以包含定义时作用域上下文的函数对象。

```C++
#include <iostream>

// 使用g++ 00_01_Lambda_Simple.cpp -fdump-tree-gimple 生成Simple文件
int main() {
    int x = 5, y = 8;
    auto foo = [=] { return x * y; };
    int z = foo();
    return 0;
}
```

用`GCC`输出其`GIMPLE`的中间代码

```C++
main ()
{
  int D.40976;

  {
    int x;
    int y;
    struct __lambda0 foo;
    typedef struct __lambda0 __lambda0;
    int z;

    try
      {
        x = 5;
        y = 8;
        foo.__x = x;
        foo.__y = y;
        z = main()::<lambda()>::operator() (&foo);
        D.40976 = 0;
        return D.40976;
      }
    finally
      {
        foo = {CLOBBER};
      }
  }
  D.40976 = 0;
  return D.40976;
}


main()::<lambda()>::operator() (const struct __lambda0 * const __closure)
{
  int D.40979;
  const int x [value-expr: __closure->__x];
  const int y [value-expr: __closure->__y];

  _1 = __closure->__x;
  _2 = __closure->__y;
  D.40979 = _1 * _2;
  return D.40979;
}


__static_initialization_and_destruction_0 (int __initialize_p, int __priority)
{
  if (__initialize_p == 1) goto <D.40981>; else goto <D.40982>;
  <D.40981>:
  if (__priority == 65535) goto <D.40983>; else goto <D.40984>;
  <D.40983>:
  std::ios_base::Init::Init (&__ioinit);
  atexit (__tcf_0);
  goto <D.40985>;
  <D.40984>:
  <D.40985>:
  goto <D.40986>;
  <D.40982>:
  <D.40986>:
}


__tcf_0 ()
{
  std::ios_base::Init::~Init (&__ioinit);
}


_GLOBAL__sub_I_main ()
{
  __static_initialization_and_destruction_0 (1, 65535);
}
```

从上面的中间代码可以看出lambda表达式的类型名为`__lambda0`，通过这个类型实例化了对象`foo`，然后在函数内对`foo`对象的成员`__x`和`__y`进行赋值，最后通过自定义的`()`运算符对表达式执行计算并将结果赋值给变量`z`。在这个过程中，`__lambda0`是一个拥有`operator()`自定义运算符的结构体，这也正是函数对象类型的特性。在某种程度上来说，lambda表达式是C++11提供的一块语法糖。



# 无状态lambda表达式

C++标准对于无状态的lambda表达式可以隐式转换为函数指针,例如:

```C++
void f(void(*)()) {}
// lambda表达式[]{}隐式转换为void(*)()类型的函数指针
void g() { f([] {}); } // 编译成功

void f1(void(&)()) {}
void g1() { f1(*[] {}); }
```

在C++20标准之前无状态的lambda表达式类型既不能构造也无法赋值，这阻碍了许多应用的实现。

```C++
// lambda表达式类型无法构造编译会报错
auto greater = [](auto x, auto y) { return x > y; };
std::map<std::string, int, decltype(greater)> mymap;

// 状态的lambda表达式无法赋值,编译报错
auto greater = [](auto x, auto y) { return x > y; };
std::map<std::string, int, decltype(greater)> mymap1, mymap2;
mymap1 = mymap2;
```



为了解决以上问题，C++20标准允许了无状态lambda表达式类型的构造和赋值。

# 广义捕获

C++14标准中定义了广义捕获，所谓广义捕获实际上是两种捕获方式

1. 第一种称为简单捕获，这种捕获就是我们在前文中提到的捕获方法，即`[identifier]`、`[&identifier]`以及`[this]`等。
2. 第二种叫作初始化捕获，在C++14标准中引入的，它解决了简单捕获的一个重要问题，即只能捕获lambda表达式定义上下文的变量，而无法捕获表达式结果以及自定义捕获变量名，比如：

```C++
void main() {
    int x = 5;
    // 即auto foo = [y = x + 1]{ return y; }
    // C++11只支持简单捕获,无法编译,C++14可以
    // 等号左边的变量x存在于lambda表达式的作用域
    // 等号右边x存在于main函数的作用域
    auto foo = [x = x + 1]{ return x; };
}
```

初始化捕获在这些场景下是非常实用的

1. 使用移动操作减少代码运行的开销

    ```C++
    {
        std::string x = "hello c++ ";
        auto foo = [x = std::move(x)]{ return x + "world"; };
    }
    ```

2. 在异步调用时复制`this`对象，防止lambda表达式被调用时因原始`this`对象被析构造成未定义的行为

    ```C++
    class Work {
    private:
        int value;
    public:
        Work() : value(42) {}
        std::future<int> spawn() {
            // 利用初始化捕获的特性,将对象复制到lambda表达式内
            return std::async([=, tmp = *this]() -> int {
                return tmp.value;
            });
        }
    };
    ```

    

# 泛型Lambda表达式

C++14标准让lambda表达式具备了模版函数的能力，我们称它为泛型lambda表达式。虽然具备模版函数的能力，但是它的定义方式却用不到template关键字，只需要使用auto占位符即可。

```C++
{
    auto foo = [](auto a) { return a; };
    int three = foo(3);
    char const* hello = foo("hello");
}
```

C++14标准中lambda表达式通过支持`auto`来实现泛型。大部分情况下，这是一种不错的特性，但这种语法也会使我们难以与类型进行互动。用提案文档的举例来说：

```C++
template <typename T>
struct is_std_vector : std::false_type { };

template <typename T>
struct is_std_vector<std::vector<T>> : std::true_type { };

auto f = [](auto vector) {
	static_assert(is_std_vector<decltype(vector)>::value, "");
};
```

普通的函数模板可以轻松地通过形参模式匹配一个实参为`vector`的容器对象，但是对于lambda表达式，`auto`不具备这种表达能力，所以不得不实现`is_std_vector`，并且通过`static_assert`来辅助判断实参的真实类型是否为`vector`。在C++委员会的专家看来，把一个本可以通过模板推导完成的任务交给`static_assert`来完成不合适。这样的语法让获取`vector`存储对象的类型也变得十分复杂。

```C++
auto f = [](auto vector) {
    // vector容器类型的内嵌类型value_type表示存储对象的类型
	using T = typename decltype(vector)::value_type;
	// …
};
```

并不能保证面对的所有容器都会实现这一规则，所以依赖内嵌类型是不可靠的。`decltype(obj)`有时候并不能直接获取我们想要的类型。

```C++
// decltype(x)推导出来的类型并不是std::vector,是const std::vector&
// T copy = x;不是一个复制而是引用
// 对于一个引用类型,T::iterator不符合语法
auto f = [](const auto& x) {
    using T = decltype(x);
    T copy = x; // 可以编译，但是语义错误
    using Iterator = typename T::iterator; // 编译错误
};

std::vector<int> v;
f(v);
```



C++20中添加模板对lambda的支持，语法非常简单:

```C++
[]<typename T>(T t) {}
```

上面的示例便可以写为

```C++
auto f = []<typename T>(std::vector<T> vector) {
	// …
};
```

```C++
auto f = []<typename T>(T const& x) {
    T copy = x;
    using Iterator = typename T::iterator;
};
```

# 常量lambda表达式和捕获*this

C++17标准对lambda表达式同样有两处增强:

1. 常量lambda表达式。
    lambda表达式可以声明为`constexpr`或在常量表达式中使用，当lambda表达式捕获或传入的数据成员都是常量表达式。
    ```C++
    int y = 32;
    auto answer = [y]() constexpr {
        int x = 10;
        return y + x;
    };

    constexpr int Increment(int n) {
        return [n] { return n + 1; }();
    }
    ```
    如果lambda表达式需要满足constexpr函数的要求，它是隐式的常量lambda表达式。
    ```C++
    auto answer = [](int n) {
        return 32 + n;
    };
    constexpr int response = answer(10);
    ```
    如果lambda表达式是隐式或显式的constexpr，并且将其转换为函数指针，则生成的函数也是 constexpr
    ```C++
    auto Increment = [](int n) {
        return n + 1;
    };
    constexpr int(*inc)(int) = Increment;
    ```

2. 对捕获`*this`的增强。`[*this]`的语法让程序生成了一个`*this`对象的副本并存储在lambda表达式内，可以在lambda表达式内直接访问这个复制对象的成员。

    ```C++
    class Work {
    private:
        int value;
    public:
        Work() : value(42) {}
        std::future<int> spawn() {
            return std::async([=, *this]() -> int {
                return value;
            });
        }
    };
    ```

    `[=]`可以捕获this指针，相似的，`[=,*this]`会捕获`this`对象的副本。代码中大量出现`[=]`和`[=,*this]`时可能很容易忘记前者与后者的区别。为了解决这个问题，在C++20标准中引入了`[=, this]`捕获`this`指针的语法，它实际上表达的意思和`[=]`相同，目的是让程序员们区分它与`[=,*this]`的不同,在C++20标准中还特别强调了要用`[=, this]`代替`[=]`。

    ```C++
    // C++20新增[=, this]捕获this指针
    // C++17 编译报错或者报警告， C++20成功编译
    [=, this]{};
    ```

# 参考资料

1. 现代C++语言核心特性解析
2. [MSDN:constexpr lambda expressions in C++](https://learn.microsoft.com/en-us/cpp/cpp/lambda-expressions-constexpr?view=msvc-170)
