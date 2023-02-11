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
当编译器需要的时候，才会合成default constructor，且被合成出的default constructor只执行编译器所需要的行为。例如，下面的`class Foo`并不会合成出Default Constructor。
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
在这个例子中，程序的语意是要求Foo有一个default constructor，该函数可以将它的成员初始化为0。上面的程序并不会合成出一个default constructor(即implicit trivial default constructors，实际上它并不会被合成出来)。而且即使有需要为`class Foo`合成一个default constructor，那个constructor也不会将`val`和`pnext`初始化为0。为了让上一段代码正确执行，`class Foo`的设计者必须提供一个显式的default constructor，将两个val和pnext正确地初始化。

C++ Standard Draft [ISO-C++11]的Section 12.1这么说：

> A default constructor for a class X is a constructor of class X that can be called without an argument. If there is no user-declared constructor for class X, a constructor having no parameters is implicitly declared as defaulted. An implicitly-declared default constructor is an inline public member of its class. 
> ....
> A default constructor is trivial if it is neither user-provided nor deleted and if:
>
> - its class has no virtual functions and no virtual base classes, and
> - no non-static data member of its class has a brace-or-equal-initializer, and
> - all the direct base classes of its class have trivial default constructors, and
> - for all the non-static data members of its class that are of class type (or array thereof), each such class
>
> has a trivial default constructor.
>
> Otherwise, the default constructor is non-trivial

也就是说：
> 如果`calss X`没有用户自定义的构造函数，默认会隐式地声明一个无参构造函数。该隐式声明的默认构造函数是该类的inline public成员函数。
> 
> 如果该构造函数既不是用户自定义的也不是deleted，且同时满足下面的条件：
> - 该class没有虚函数且无虚基类，且
> - ....
> - ....
> - ...
> 
> 类的默认构造函数是trivial(无用的)
> 
> 否则，类的默认构造函数是non-trivial。

下面讨论non-trivial default constructor的4种情况:
### 带有default constructor的Member Class Object

**如果一个class没有任何constructor，但它内含一个member object，而后者有default constructor，那么该class的implicit default constructor就是non-trivial，编译器需要为该class合成出一个default constructor，该合成操作只有在constructor真正需要被调用时才会发生。**

于是出现了一个问题：在C++各不同的compile module中，编译器如何避免合成出多个default constructor（比如说一个是为A.c文件合成，另一个是为B.c文件合成）呢？解决方法是把合成的default constructor、copy constructor、destructor、assignment copy operator都以inline方式完成。一个inline函数有静态链接(static linkage),不会被文件以外者看到。如果函数太复杂，不适合做成inline，就会合成出一个explicit non-inlinestatic实例.


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
**被合成的defaultconstructor只满足编译器的需要**，而不是程序的需要。

为了保证程序正确执行，`Bar::str`也需要被初始化，假设我们定义了构造函数
```C++
// 用户自定义构造函数
Bar::Bar() {
    str = 0;
}
```
由于由于defaultconstructor已经被显式地定义出来，编译器没办法合成第二个。可是`Bar::foo`还是需要初始化，那么编译器会采取什么行动呢？编译器会执行“**如果class A内含一个或一个以上的member class objects，那么 class A的每一个constructor必须调用每一个member classes的default constructor”。编译器会扩张已存在的constructors，在其中安插一些代码，使得用户代码被执行之前，先调用必要的default constructors。** 延续前一个例子，扩张后的constructors可能像这样：
```C++
Bar::Bar() {
    // compiler code
    foo.Foo::Foo();

    // explicit user code
    str = 0;
}
```

如果有多个class member objects都要求constructor初始化操作，将如何？**C++语言要求编译器以member objects在class中的声明顺序来调用各个constructors。编译器为每一个constructor插入代码，以“member声明顺序”调用每一个member所关联的default constructors。** 这些代码将放在explicit usercode之前。例如：
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

### “带有 Default Constructor”的 Base Class

**如果一个没有任何constructors的class派生自一个“带有default constructor”的base class，那么这个derived class的default constructor会被视为non-trivial，并因此需要被合成出来。它将调用上一层 base classes 的 default constructor(根据声明顺序)。**对于后一个派生类而言，该合成的default constructor和用户显时提供的default constructor没有区别。

如果设计者提供多个constructors，但没有default constructor呢？由于用户已经自定了constructors，那么编译器便不会再合成编译器default constructor。编译器会扩张现有的每一个constructors“来调用所有必要的default constructors”的代码。如果同时还存在”带有default constructors”的member class objects，在所有base class constructor都被调用之后，这些member class的default constructor也会被调用。

### “带有一个 Virtual Function”的 Class

下面两种情况也需要合成default constructor：

1. class声明（或继承）一个 virtual function。
2. class派生自一个继承串，其中有一个或更多的 virtual base classes。

不管哪一种情况，由于缺乏用户自定声明的constructors，编译器会详细记录合成一个defaultconstructor的必要信息。以下面的程序为例：

```C++
class Widget {
public:
    virtual void flip() = 0;
    // ...
};

void flip( const Widget& widget ) {
    widget.flip();
}

// 假设 Bell 和 Whistle 都派生自 Widget
void foo() {
    Bell b;
    Whistle w;
    flip(b);
    flip(w);
}
```



下面两个扩张行动会在编译期间发生：

1. 一个 virtual function table(在 cfront中被称为 vtbl)会被编译器成成出来，内放class的virtual functions地址。
2. 在每一个class object中，一个额外的 pointer member（也就是 vptr）会被编译器合成出来，内含相关之 class vtbl的地址。

此外，`widget.flip()`的虚拟调用操作（virtual invocation）会被重新改写，以使用`widget`的vptr和vtbl中`flip()`

```C++
// widget.flip() 的虚拟调用操作（virtual invocation）的转变
(*widget.vptr[1])(&widget); 
```

其中：

1. 索引1()在 virtual table中的固定索引
2. `&widget`代表要交给“被调用的某个`flip()`函数实例”的 this指针

为实现这个机制，编译器必须为每一个 Widget(或其派生类的) object的vptr设定初值，放置适当的virtual table地址。对于class定义的每一个constructor，编译器会插入一些代码来做这样的事情。对于那些未声明任何constructors的classes，编译器会合成一个default constructor，以便正确地初始化每一个class的vptr。

### “带有一个 Virtual Base Class”的 Class

Virtual base class的实现法在不同的编译器之间有极大的差异。每一种实现法的共同点在于必须使virtual base class在其每一个derived class object中的位置，在执行期准备妥当。例如下面的代码：

```C++
class X {
public:
    int i;
};

class A : public virtual X {
public:
    int j;
};

class B : public virtual X {
public:
    double d;
};

class C : public A, public B {
    public: int k;
};

// 无法在编译时期决定（resolve）出 pa->X::i 的位置
void foo( const A* pa )  { 
	pa->i = 1024;  
}

void main() {
    foo(new A);
    foo(new C);
    // ...
}
```

对于`foo`函数，由于`pa`的真实类型会发生改变，译器无法固定`foo()`中“经由`pa`而存取的`X::i`”的实际偏移位置。**编译器必须改变“执行存取操作”的那些代码，使`X::i`延迟至执行期才决定下来。**原先cfront的做法是靠“在derived class object的每一个virtual base classes中插入一个指针”完成。所有“经由reference或pointer来存取一个virtual base class”的操作都可以通过相关指针完成。在这个例子中，`foo()`可以被改写如下，以符合这样的实现策略：

```C++
// 可能的编译器转换操作
// 
void foo( const A* pa ) {
    pa->__vbcX->i = 1024;
}
```

其中`__vbcX`表示编译器所产生的指针，指向virtual base class X。`__vbcX`（或编译器所做出的某个东西）是在 class object 构造期间完成的。对于 class 所定义的每一个constructor，**编译器会插如那些“允许每一个virtual base class的执行期存取操作”的代码**。如果class没有声明任何constructors，编译器必须为它合成一个default constructor。



综上，有4种情况会造成“编译器必须为未声明constructor的classes合成一个defaultconstructor”。C++ Standard 把那些合成物称为implicit non-trivial default constructors。被合成出来的constructor只能满足编译器(而非程序)需要。它借着调用

- member object或base class的default constructor”或
- “为每一个object初始化其virtual function机制或
- virtual base class机制”

来完成。至于没有存在那4种情况而且没有显式声明任何constructor的classes，他们拥有的是implicit trivial default constructors，实际上并不会被合成出来。

在合成的 default constructor 中，只有 base class subobjects 和 member class objects会被初始化。所有其他的nonstatic data member(如整数、整数指针、整数数组等等)都不会被初始化。这些初始化操作对程序而言可能有需要，但对编译器则非必要。如果程序需要一个“把某指针设为0”的default constructor，那么程序员应该提供。

# 参考资料

- 《深度探索C++对象模型》