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



综上，有4种情况会造成“编译器必须为未声明constructor的classes合成一个default constructor”。C++ Standard 把那些合成物称为implicit non-trivial default constructors。被合成出来的constructor只能满足编译器(而非程序)需要。它借着调用

- member object或base class的default constructor”或
- “为每一个object初始化其virtual function机制或
- virtual base class机制”

来完成。至于没有存在那4种情况而且没有显式声明任何constructor的classes，他们拥有的是implicit trivial default constructors，实际上并不会被合成出来。

在合成的 default constructor 中，只有 base class subobjects 和 member class objects会被初始化。所有其他的nonstatic data member(如整数、整数指针、整数数组等等)都不会被初始化。这些初始化操作对程序而言可能有需要，但对编译器则非必要。如果程序需要一个“把某指针设为0”的default constructor，那么程序员应该提供。

## Copy Constructor的构造操作

有三种情况会以一个object的内容作为另一个class object的初值。

1. 用一个object的作为另外一个class object的初值

```C++ 
class X { ... };

X x;
X xx = x;
```

2. 将object当作参数传递给函数

```C++
extern void foo(X x);
void bar(){
    X xx;        // 以 xx作为foo()第一个参数的初值（隐式的初始化操作）
    foo( xx );
    // ...
}
```

3. 函数返回一个class object。

```C++
X foo_bar() {
    X xx;
    // ...
    return xx;
}
```

如果class中显式定义了一个copy constructor(有一个参数的类型是其class type的引用)，像下面这样：

```C++
// user-defined copy constructor示例
X::X(const X& x);
// 可以是多参数形式，第二个参数及其以后的参数都有默认值
Y::Y(const Y& y, int idex = 0);
```

那么在大部分情况下，当一个 class object以另一个同类实例作为初值，上述的constructor会被调用。

### Default Memberwise Initialization

**如果class没有提供一个explicit copy constructor又当如何？当class object以“相同 class 的另一个 object”作为初值，其内部是以所谓的 default memberwise initialization手法完成的，也就是把每一个内建的或派生的data member（例如一个指针或一个数组）的值，从某个object拷贝一份到另一个object身上。不过它并不会拷贝其中的member class object，而是以递归的方式施行memberwise initialization。**例如，下面的class声明：

```C++
class String {
public:
    // ....  没有显式地定义copy constuctor
private:
    char* str;
    int len;
};
```

`String object`的default memberwise initialization发生在这种情况

```C++
String noun("book");
String verb = noun;
```

其完成方式就像分别设定每一个成员一样：

```C++
verb.str = noun.str;
verb.len = noun.len;
```



如果String object中声明了另一个class的member，类似于下面这样

```C++
class Word {
public:
    // ....  没有显式地定义copy constuctor
private:
    int		__occurs;
    // String是class Word的一个成员
    String	_word;
};
```

那么`Word object`的default memberwise initialization会拷贝其内建的member `_occurs`，然后再于 `String` member object `_word` 上递归实施 memberwise initialization。这样的操作如何实现呢？

### Bitwise Copy Semantics(位逐次拷贝)



## 程序转化语义学(Program Tansformation Semantics)

下面的程序:

```C++
X foo() {
    X xx;
    // ...
    return xx;
}
```

有人可能会做出如下假设:

1. 每次`foo()`被调用，就传回`xx`的值。--是否正确， 视`class X`如何定义而定。
2. ．如果`class X`定义了一个 copy constructor，那么当`foo()`被调用时，保证该copy constructor也会被调用。-- 是否正确， 视`class X`如何定义而定，最主要的是视采用的C++编译器所提供的degree of aggressive optimization而定。

### 显示初始化操作(Explicit Initialization)

### 参数的初始化(Argument Initialization)

### 返回值的初始化(Return Value Initialization)

### 在使用者层面做优化(Optimization at the User Level)



### 编译器层面做优化(Optimization at the Compiler Level)

像`bar()`这样的函数中，所有的return指令传回相同的具名数值(named value，应该指的的是`xx`)因此编译器有可能以result参数取代named return valued的方式进行优化。例如下面的`bar()`定义

```C++
X foo() {
    X xx;
    // ...
    return xx;
}
```

编译器把其中的xx以`__result`替换

```C++
void foo(X& __result) {
    // 调用默认构造
    // C++伪码
    __result.X::X();
    // ... 直接处理__result
}
```

这样的编译器优化操作，被称作**Named Return Value(NRV)优化**。



考虑下面的实现

```C++
class test {
	friend test foo(double);
public:
    test() {
        memset(array, 0, 100 * sizeof(double));
    }
    
private:
    double array[100];
};
```

同时考虑下面的实现，它产生、修改并传回一个`test` class oject:

```C++
test foo(double val) {
    test local;
    local.array[ 0 ] = val;
    local.array[ 99 ] = val;
    return local;
}
```

有一个`main()`函数调用上述`foo()`函数1000万次：

```C++
void main() {
    for (int cnt = 0; cnt < 10000000; cnt++) {
        test t = foo(double(cnt));
    }
}
```

另外一个版本给`class test`添加一个inline copy constructor来激活C++编译器中的NRV优化

```C++
inline test::test(const test& t) {
    memcpy(this, &t, sizeof(test)
);      
```

作者给出了测试时间表

|      | 未实施NRV | 实施NRV | 实施NRV+-o |
| ---- | --------- | ------- | ---------- |
| CC   | 1:48.52   | 46.73   | 46.05      |
| NCC  | 3:00.57   | 1:33.48 | 1:32:36    |

NRV虽然改善了效率，但却饱受批评。其中一个原因是，它是由编译器默默完成的，而它是否真的完成，并不十分明确(因为很少有编译器会说明其实现程度，或是否实现)。第二个原因是如果函数比较复杂，优化就变得难以实行。在cfront中，只有当所有的named return指令句发生于函数的top level，优化才施行。如果导入“a nested local block with a return statement”, cfront 就会静静地将优化关闭。

第三个批评则是某些程序员并不喜欢应用程序被优化。想象你准备好了copy constructor，期待程序以copying方式产生出一个object，对称地调用destructor，NVR优化会打破这种对称性。例如：

```C++
void foo() {
    // 这里希望有一个copy constructor
    X xx = bar();
    // ...
    // 这里调用destructor
}
```

VRV优化虽然提高了程序的执行效率，但是结果却是错误的。



那么，copy constructor在“object是经由copy而完成其初始化”的情况下，一定要被调用吗？这样在很多程序中会被征以严格的“效率税”。例如，虽然下面三个初始化操作在语意上相等：

```C++
X xx0(1024);
X xx1 = X(1024);
X xx2 = (X) 1024;
```

`xx0`是被单一的constructor操作设定初值：

```C++
// C++ 伪码
xx0.X::X(1024 );
```

而`xx1`或`xx2`是调用constructor产生一个临时object，并将临时性的 object以拷贝构造的方式作为`xx1/xx2`的初值，再针对该临时object调用析构函数

```C++
// C++伪码
X __temp0;
__temp0.X::X(1024);
// copy constructor
xx1.X::X(__temp0 );
// destructor
__temp0.X::~X();
```

C++标准委员会已经讨论过“剔除copy constructor调用操作”的合法性。目前还没有明确决议。此时需要考虑下面两种情况：

是否copy constructor的剔除在“拷贝static object和local object”时也应该成立？例如下面的程序:

```C++
Thing outer
{
    // 是否可以不考虑inner？
    Thing inner(outer); // automatic objects
}
```

[cpp reference:copy constructor](https://en.cppreference.com/w/cpp/language/copy_constructor)中有如如下的描述:

> ### Deleted implicitly-declared copy constructor
>
> The implicitly-declared copy constructor for class `T` is undefined if any of the following conditions are true:  (until C++11)
>
> The implicitly-declared or defaulted copy constructor for class `T` is defined as *deleted* if any of the following conditions are true:(since C++11)
>
> - `T` has non-static data members that cannot be copied (have deleted, inaccessible, or ambiguous copy constructors);
> - `T` has direct or virtual base class that cannot be copied (has deleted, inaccessible, or ambiguous copy constructors);
> - `T` has direct or virtual base class or a non-static data member with a deleted or inaccessible destructor;
>
> Following（since C++11）
>
> - `T` is a union-like class and has a variant member with non-trivial copy constructor;
> - `T` has a data member of rvalue reference type;
> - `T` has a user-defined move constructor or move assignment operator (this condition only causes the implicitly-declared, not the defaulted, copy constructor to be deleted).

一般而言，面对“以一个class object作为另一个class object的初值”的情形，语言允许编译器有大量的自由发挥空间，这可能会带来明显的效率提升。缺点则是你不能够安全地规划你的copy constructor的副作用，必须视其执行而定。






## 成员们的初始化(Member Initialization List)

当我们实现constructor时，就有机会设定class members的初值。可以经由member initialization list，也可以在constructor函数内进行实现。

下面几种情况，必须使用member initialization list初始化，否则程序无法顺利编译：
1. 当初始化一个reference member时；
2. 当初始化一个const member时；
3. 当调用一个base class的 constructor，而它拥有一组参数时；
4. 当调用一个member class的 constructor，而它拥有一组参数时。

```C++
class Word {
    String  _name;
    int     _cnt;
public:
    Word() {
        _name = 0;
        _cnt = 0;
    }
};
```

`Word` constructor会先产生一个临时性的`String` object，然后将它初始化，之后以一个assignment运算符将临时性object指定给`_name`，随后再销毁哪个临时的object。下面是constructor可能的扩展结果
```C++
Word::Word(/*this pointer goes here*/) {
    // 调用String的default constructor
    _name::String::String();

    // 产生临时性对象
    Sting temp = String(0);
    // memberwise地copy给_name
    _name.String::operator=(tmp);
    // 销毁临时性对象
    temp.String::~String();

    _cnt = 0;
}
```
对代码反复审查和修正，得到一个更有效的实现方法
```C++
Word::Word:_name(0) {
    _cnt = 0;
}
```
该代码会扩张成这个样子：
```C++
Wor::Word(/*this pointer goes here*/) {
    // 调用String(int) constructor
    _name.String::String(0);

    _cnt = 0;
}
```

<font color=blue><b>
编译器会一一操作initialization list，以class中member声明顺序在constructor之内插入初始化操作，并且在任何explicit user code之前。<b></font>

初始化顺序”和“initialization list中的项目排列顺序”之间的外观错乱，会导致下面意想不到的危险：
```C++
class X {
    int i;
    int j;
public:
    X(int val) : j(val), i(j) {}
    // ....
};
```
由于声明顺序，initialization list中的`i(j)`比`j(val)`更早执行。但因为j
一开始未有初值，所以`i(j)`的执行结果导致i无法预知其值。

因为initialization list的项目被放在explicit user code之前，所以下面的实现可以得到预期的效果.
```C++
// j的初始化操作会被安插在explicit user assignment操作之前
// 可以得到j=val， i=j的预期
X::X(int val) : j(val) {
    i = j;
}
```

另一个常见的问题是，你是否能够像下面这样，调用一个member function以设定一个member的初值：
```C++
// X::xfoo()被调用，这样好吗？
X::X(int val) : i(xfoo(val)), j(val) {
}
 ```

其中`xfoo()`是`X`的一个member function。答案是yes，但是，要注意：<font color=blue><b>请使用“存在于constructor体内的一个member”，而不要使用“存在于member initialization list中的member”，来为另一个member设定初值。</b></font>你并不知道`xfoo()`对`X` object的依赖性有多高，如果把`xfoo()`放在constructor内，那么对于“到底是哪一个member在`xfoo()`执行时被设立初值”这件事，就可以确保不会发生模棱两可的情况。
```C++
// C++伪码
X::X(/*this pointer*/, int val) {
    i = this->xfoo(val);
    j = val;
}
```


如果一个derived class member function被调用，其返回值被当做base class constructor的一个参数，将会如何：
```C++
class FooBar ： public X {
    int _fval;
public:
    int fval() {
        return _fval;
    }

    FooBar(int val) ： _fval(val), 
        // fval()作为base class constructor的参数
        X(fval()) {}
};
```
它可能的扩展结果：
```C++
FooBar::FoorBar(/*this pointer*/, int val) {
    // oops, bad idea
    X::X(this, this->fval());
    _fval = val;
}
```

简略地说，编译器会对 initialization list一一处理并可能重新排序，以反映出members的声明顺序。它会插入一些代码到constructor内，并置于任何explicit user code之前


# 参考资料

- 《深度探索C++对象模型》