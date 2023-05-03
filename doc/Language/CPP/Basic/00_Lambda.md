

从C++11开始支持lambda表达式。

```C++
[ captures ] ( params ) specifiers exception -> ret { body }
```

`[captures]`： 捕获列表，它可以捕获当前函数作用域的零个或多个变量，变量之间用逗号分隔。捕获列表的捕获方式有两种：按值捕获和引用捕获。Lambda表达式的列表捕获方法除了指定捕获变量之外，还有：

- `[this]`：捕获`this`指针，让我们使用`this`类型的成员变量和函数。
- `[=]`：捕获Lambda表达式定义作用域的全部变量的值，包括`this`。
- `[&]`：捕获Lambda表达式定义作用域的全部变量的引用，包括`this`。

`(params)`：可选参数列表，和普通函数的参数列表一样。

`specifiers`：可选限定符，C++11中可以用`mutable`，它允许我们在lambda表达式函数体内改变按值捕获的变量，或者调用非`const`的成员函数。

`exception` ：可选异常说明符，我们可以使用`noexcept`来指明lambda是否会抛出异常。

`ret`：可选返回值类型。不同于普通函数，lambda表达式使用返回类型后置的语法来表示返回类型，如果没有返回值(void类型)，可以忽略包括->在内的整个部分。

`body`：Lambda表达式的函数体。

## 捕获列表

捕获列表中的变量存在于两个作用域——Lambda表达式定义的函数作用域以及Lambda表达式函数体的作用域，前者是为了捕获变量，后者是为了使用变量。标准还规定能捕获的变量必须是一个自动存储类型，也就是就是非静态的局部变量。如果在Lambda表达式需要使用全局变量或者静态局部变量，可以直接使用。

```C++
int x = 0;

int main() {
    int y = 0;
    static int z = 0;
    // 由于x和z不是自动存储类型的变量
    // 且x不在lambda表达式定义的作用域中
    // 无法正常编译
    // auto foo = [x, y, z] {};
    
    // 在Lambda表达式中直接使用全局变量或者静态局部变量
    auto foo = [y] { return x + y + z; };
}
```

由于捕获列表的变量必须是一个自动存储类型，但是全局作用域并没有这样的类型。如果我们将一个Lambda表达式定义在全局作用域，那么Lambda表达式的捕获列表必须为空。

```C++
int x = 1;
auto foo = [] { return x; };
int main() {
    foo();
}
```



示例，捕获`this`指针。

```C++
#include <iostream>

class A {
public:
    void print() {
        std::cout << "class A ( x = " << x << ")" << std::endl;
    }

    void test() {
        auto foo = [this] { 
            print(); 
            x = 5; 
        };
        foo();
        print(); 
    }
private:
    int x;
};

// class A ( x = 0)
// class A ( x = 5)
int main() {
    A a;
    a.test();
    return 0;
}
```

```C++
#include <iostream>

int main() {
    int x = 5, y = 8;
    // 捕获Lambda作用域内的所有变量的值
    auto foo = [=] { return x * y; };
    std::cout << foo() << std::endl;
    
    // 捕获Lambda作用域的全部变量的引用
    auto foo2 = [&] { ++x, ++y; };
    std::cout << "x = " << x << ", y = " << y << std::endl;
    return 0;
}
```
### 按值捕获和引用捕获
lambda表达式的一个特性：捕获的变量默认为常量，或者说lambda是一个常量函数（类似于常量成员函数）.
```C++
void bar1()
{
    int x = 5, y = 8;
    auto foo = [x, y] {
        x += 1;             // 编译失败，无法改变捕获变量的值
        y += 2;             // 编译失败，无法改变捕获变量的值
        return x * y;
    };
    std::cout << foo() << std::endl;
}

void bar2() {
    int x = 5, y = 8;
    auto foo = [&x, &y] {
        x += 1;
        y += 2;
        return x * y;
    };
    std::cout << foo() << std::endl;
}
```

使用mutable说明符可以移除lambda表达式的常量性
```C++
void bar3() {
    int x = 5, y = 8;
    auto foo = [x, y] () mutable {
        x += 1;
        y += 2;
        return x * y;
    };
    std::cout << foo() << std::endl;
}
```

捕获值和捕获引用还是存在着本质区别。当lambda表达式捕获值时，表达式内实际获得的是捕获变量的复制，我们可以任意地修改内部捕获变量，但不会影响外部变量。而捕获引用则不同，在lambda表达式内修改捕获引用的变量，对应的外部变量也会被修改：
```C++
#include <iostream>
/**
    lambda x = 6, y = 10
    call1  x = 5, y = 10
    lambda x = 7, y = 12
    call2  x = 5, y = 12
*/
int main() {
    int x = 5, y = 8;
    auto foo = [x, &y]() mutable {
        x += 1;
        y += 2;
        std::cout << "lambda x = " << x << ", y = " << y << std::endl;
        return x * y;
    };
    foo();
    std::cout << "call1  x = " << x << ", y = " << y << std::endl;
    foo();
    std::cout << "call2  x = " << x << ", y = " << y << std::endl;
}
```

捕获值的变量在lambda表达式定义的时候已经固定下来了，无论函数在lambda表达式定义后如何修改外部变量的值，lambda表达式捕获的值都不会变化，
```C++
#include <iostream>

// lambda x = 6, y = 22
int main() {
    int x = 5, y = 8;
    auto foo = [x, &y]() mutable {
        x += 1;
        y += 2;
        std::cout << "lambda x = " << x << ", y = " << y << std::endl;
        return x * y;
    };
    x = 9;
    y = 20;
    foo();
}
```

# 参考资料

1. 现代C++语言核心特性解析