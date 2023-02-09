# 对象模型:构造函数语意学

Jerry Schwarz, iostream函数库的建筑师，就曾经说过一个故事：他最早的意图是支持一个iostream class object的标量测试(scalar test)：

```C++
if (cin)
    ...
```



编译器，比较喜欢找到一个正确的诠释（如果有的话），而不只是把程序标示为错误就算了！在此例中，内建的左移位运算符（left shiftoperator, <<）只有在“cin
可改变为和一个整数值同义”时才适用。编译器会检查可以使用的各个conversion运算符，然后它找到了operator int()
，那正是它要的东西。左移位运算符现在可以操作了


## Default Constructor构造操作
------------------------------ to make sure ------------------------------

C++ Annotated Reference Manual(ARM)[ELLIS-C++95]的Section 12.1这么说：
> “defaultconstructors……在需要的时候被编译器产生出来

------------------------------ to make sure ------------------------------

```c++
class Foo {
public:
    int val;
    Foo* pnext;
};

void foo_bar() {
    Foo bar; // opps，程序要求bar的成员都被清为0
    if (bar.val || bar.pnext) {
        // .... do something
    }
}
```
在这个例子中，程序的语意是要求Foo有一个default constructor，该函数可以将它的成员初始化为0。上面的程序并不会合成出一个default constructor。

那么，什么时候才会合成出一个default constructor?当编译器需要的时候，而且被合成出的constructor只执行编译器所需要的行为。也就是说，即使有需要为`class Foo`合成一个default constructor，那个constructor也不会将val
和pnext初始化为0。为了让上一段代码正确执行，`class Foo`的设计者必须提供一个显式的default constructor，将两个val和pnext正确地初始化。

------------------------------ to make sure ------------------------------

C++ Standard [ISO-C++95]的Section 12.1这么说：
> 对于 class X，如果没有任何 user-declaredconstructor，那么会有一个 default constructor被隐式（implicitly）声明出来……一个被隐式声明出来的 default constructor将是一个trivial（浅薄而无能，没啥用的）constructor……

------------------------------ to make sure ------------------------------

non-trivial default constructor在ARM的术语中就是编译器所需要的，必要时候由编译器合成出来。下面讨论non-trivial default constructor的4种情况:

## 带有default constructor的Member Class Object

如果一个class没有任何constructor，但它内含一个member object，而后者有default constructor，那么该class的implicit default constructor就是nontrivial，编译器需要为该class合成出一个default constructor，该合成操作只有在constructor真正需要被调用时才会发生。

于是出现了一个问题：在C++各不同的compile module中，编译器如何避免合成出多个default constructor（比如说一个是为A.c文件合成，另一个是为B.c文件合成）呢？解决方法是把合成的default constructor、copy constructor、destructor、assignment copy operator都以inline方式完成。一个inline函数有静态链接（static linkage），不会被文件以外者看到。如果函数太复杂，不适合做成inline，就会合成出一个explicit non-inlinestatic实例.


```C++
class Foo {
public:
    Foo();
    Foo(int);

public:
    int val;
    Foo* pnext;
};

class Bar {
public:
    Foo foo;
    char* str;
};

void foo_bar() {
    Bar bar;
    if (str) {
        // ... do something
    }
}
```

被合成的Bar的default constructor内含必要的代码，能够调用`class Foo`的default constructor来处理`Bar::foo`，但它并不产生任何代码来初始化`Bar::str`。将`Bar::foo`初始化是编译器的责任，将`Bar::str`初始化则是程序员的责任。被合成的default constructor看起来可能像这样:
```C++
// Bar的default constructor可能会被这样合成
// 为member foo调用class Foo的default constructor
inline Bar::Bar() {
    // C++伪码
    foo.Foo::Foo();
}
```
被合成的defaultconstructor只满足编译器的需要，而不是程序的需要。

为了保证程序正确执行，`Bar::str`也需要被初始化，假设我们定义了构造函数
```C++
// 用户自定义构造函数
Bar::Bar() {
    str = 0;
}
```
由于由于defaultconstructor已经被显式地定义出来，编译器没办法合成第二个。可是`Bar::foo`还是需要初始化，那么编译器会采取什么行动呢？

编译器的行动是：“如果class A内含一个或一个以上的member class objects，那么 class A的每一个 constructor 必须调用每一个 member classes的 default constructor”。。编译器会扩张已存在的constructors，在其中安插一些代码，使得user code被执行之前，先调用必要的defaultconstructors。延续前一个例子，扩张后的constructors可能像这样：
```C++
Bar::Bar() {
    // compiler code
    foo.Foo::Foo();

    // explicit user code
    str = 0;
}
```

如果有多个class member objects都要求constructor初始化操作，将如何？C++语言要求编译器以“member objects在class中的声明顺序”来调用各个constructors。编译器为每一个constructor插入代码，以“member声明顺序”调用每一个member所关联的default constructors。这些代码将放在explicit usercode之前。例如：
```C++
class Dopey {
public:
    Dopey();
};

class Sneezy {
public:
    Sneezy(int);
    Sneezy();
    ...
};

class Bashful {
public:
    Bashful();
    ...
};


class Snow_White {
public:
    Dopey dopey;
    Sneezy sneezy;
    Bashful bashful;
    // ...
private:
    int mumble;
};
```
如果`Snow_White`没有定义default constructor，就会有一个non-trivial constructor被合成出来，依序调用`Dopey`、`Sneezy`、`Bashful`的defaultconstructors,也就是
```C++
Snow_White::Snow_White() {
    // compiler code
    depey.Dopey::Dopey();
    sneezy.Sneezy::Sneezy();
    bashful.Bashful::Bashful();
}
```

如果`Snow_White`定义了下面这样的default constructor：
```C++
// 用户自定义default constructor
Snow_White::Snow_White() : sneezy(1024) {
    mumble = 2048;
}
```
编译器将其扩张为
```C++
Snow_White::Snow_White() : sneezy(1024) {
    // compiler code
    depey.Dopey::Dopey();
    sneezy.Sneezy::Sneezy(1024);
    bashful.Bashful::Bashful();

    // explicit user code
    mumble = 2048;
}
```