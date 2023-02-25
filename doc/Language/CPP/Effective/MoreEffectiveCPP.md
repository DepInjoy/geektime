# 性能
## 条款17:考虑使用lazy evaluation(缓式评估)
lazy evaluation(缓式评估), 就是以某种方式撰写你的classes，使它们延缓运算，直到那些运算结果刻不容缓地被迫切需要为止。如果其运算结果一直不被需要，运算也就一直不执行
lazy evaluation在很多场合排上用途，下面描述它的4种用途有

1. Reference Counting(引用计数)
```C++
class String { ... };    // 一个字符串类（标准的string 类型也许便是以
                        // 稍后所描述的技术实现的，不过并非一定如此)

String s1 = "Hello";
String s2 = s1;            // 调用 String的copy constructor
```
String copy constructor的一个常见做法是，一旦s2以s1为初值，便导致s1和s2 各有自己的一份“Hello”副本,z这样会导致相当大的代价，因为必须为s1的内容做一份副本，交给s2，通常会伴随着用new operator分配heap内存，并调用 strcpy将s1数据复制到s2所分配的内存内。这是所谓的eager evaluation(急式评估)：只因String的copy constructor被调用，就为s1做一个副本并放进s2内。由于s2尚未被使用，并不真正需要实际内容。

缓式（lazy）做法可以省下许多工作。我们让s2 分享s1的值，而不再给予s2一个“s1 内容副本”。唯一需要做的就是一些记录工作，使我们得以知道谁共享了些什么东西，节省了“调用 new”及“复制任何东西”的高昂成本。"s1和s2共享同一个数据结构"对客户端而言是透明的，对以下语句当然也不会有影响，因为只是读取数据，并不写入数据：
```C++
cout << s1;                 // 读出 s1的值
cout << s1 + s2;           // 读 s1和s2的值
```
事实上，数据共享所引起的唯一危机是在其中某个字符串被修改时发生的。此时应该（我们期望）只有一个（而非两个）字符串被修改。下面的语句中
```C++
s2.convertToUpperCase();
```
应该只有 s2的内容被改变，s1 并没有改变。我们需要为s2的内容做一个副本，并在修改它之前先让该副本成为s2的私有数据。

为了处理这样的语句，我们必须令 String的convertToUpperCase 函数为在convertToUpperCase 函数内，我们再不能够做任何拖延了：我们必须将s2（被共享的）的内容做一个副本，给 s2 私人使用。另一方面，如果 s2 从未被更改，我们就不需要为其内容做一个私有副本，该内容可以继续被共享。如果我们够幸运， s2 从未被修改，那么我们就不需要求取其值。
这种“数据共享”的行动细节（及相应代码）在条款29有详细叙述，其观念便是 lazy evaluation：在你真正需要之前，不必着急为某物做一个副本。取而代之的是，以拖延战术应付之——只要能够，就使用其他副本。在某些应用领域，你常有可能永远不需要提供那样一个副本。

2. 区分读和写
```C++
Strng s = "Homer＇s Iliad";     // 假设s是个reference-counted字符串。
    ...
cout << s[3];                   // 调用 operator[] 以读取数据 s[3]
s[3] = ＇x＇;                   // 调用 operator[] 将数据写入 s[3]
```
第一个 operator[] 调用动作用来读取字符串的某一部分，第二个调用则执行一个写入动作。我们希望能够区分两者，因为对一个reference-counted 字符串做读取动作，代价十分低廉，但是对这样一个字符串做写入动作，可能需要先为该字符串做出一个副本。
为了达成我们所想要的，必须在operator[]内做不同的事情（视它用于读取功能或写入功能而定）。我们如何能够判断 operator[]是在读或写的环境下被调用的呢？答案很残忍：无能为力。然而如果运用 lazyevaluation 和条款30所描述的proxy classes，我们可以延缓决定“究竟是读还是写”，直到能够确定其答案为止。


1. Lazy Fetching(缓式取出)
假设你在使用一个包含很多字段的大型对象，由于改对象必须在每次执行时保持和前次执行的一致性和连贯性，将其存储于数据库内，每个对象都有唯一的对象识别码，可用来从数据库中取回对象。
```C++
class LargeObject {                     // 大型的、可持久存在的（persistent）对象。
public:
    LargeObject(ObjectID id);           // 从磁盘中回存对象
    const string& field1() const;       // 字段 1的值
    int field2() const;                 // 字段 2的值
    double field3() const;              //……
    const string& field4() const;
    const string& field5() const;
        ...
};
```
此问题的缓式(lazy)做法是，我们在产生一个LargeObject对象时，只产生该对象的“外壳”，不从磁盘读取任何字段数据。当对象内的某个字段被需要了，程序才从数据库中取回对应的数据。下面可实现出这种"demand-paged"式的对象初始化行为：
```C++
class LargeObject {
public:
    LargeObject(ObjectID id);
    const string& field1() const;
    int field2() const;
    double field3() const;
    const string& field4() const;
    ...

private:
    ObjectID oid;
    mutable string      *field1Value;
    mutable int         *field2Value;
    mutable double      *field3Value;
    mutable string      *field4Value;
        ...
};
LargeObject::LargeObject(ObjectID id) : oid(id), field1Value(0),
    field2Value(0),field3Value(0), ... {} 
    const string& LargeObject::field1() const {
        if (field1Value == 0) {
            // read the data for field 1 from the database and make field1Value point to it;
        }
        return *field1Value; 
   }
```
null指针可能会在任何 member functions（包括const member functions，如 field1）内被赋值，以指向真正的数据。然而当你企图在 const member functions 内修改 data members，编译器不会同意。所以你必须用某种方法告诉编译器说：“放轻松，我知道我正在干什么”。说这句话的最好方法就是将指针字段声明为 mutable，意思是这样的字段可以在任何member function 内被修改，甚至是在 const member functions 内（见条款E21）。这就是为什么上述LargeObject的所有字段都被声明为 mutable的缘故。

关键词 mutable 很晚才加入 C++ 标准内，所以你的编译器厂商很可能尚未支持它。果真如此，你需要另一种方法来让编译器同意你在 const member functions 内修改 data members。一个可行的办法是所谓的“冒牌 this ”法：产生一个pointer-to-non-const指向 this 所指对象。当你需要修改某个 data member 时，就通过这个冒牌的this 指针来进行：

```C++
class LargeObject {
    public:
    const string& field1() const;
        ...
private:
    string *field1Value;            // 不再使用mutable，来支持不支持mutable的旧编译器
};

const string& LargeObject::field1() const {
    // 声明一个名为 fakeThis的指针，指向 this 所指对象
    // 并先将该对象的常量性（constness）转型掉
    LargeObject *const fakeThis = const_cast<LargeObject＊ const>(this); 
    if (field1Value == 0) {
        fakeThis->field1Value =    // the appropriate data；
    }
    return *field1Value;
}
```

4. Lazy Expression Evaluation(表达式缓评估)
考虑数值应用
```C++
template<class T>
class Matrix { ... };                // 同质（homogeneous）矩阵
Matrix<int> m1(1000, 1000);         // 一个 1000×1000的矩阵
Matrix<int> m2(1000, 1000);         // 同上
...
Matrix<int> m3 = m1 + m2;           // 将m1 加上 m2。
```
operator+ 通常采用eager evaluation(急式评估)。在这里也就是马上计算并返回m1和m2的总和。假设这是一个大规模运算(1000000)，还有大量的内存分配成本。
假设在m3被使用前，执行下面动作
```C++
Matrix<int> m4(1000, 1000);
    ...
m3 = m4 * m1;
```
那么我们便不需要`m1 + m2`的值，`m3`是`m4 * m1`。

如果要对获取`m3`中的所有内容，我们便需要计算`m3`。
```c++
cout << m3;           // 输出 m3的所有内容。
```
如果 m3 所依持的矩阵之中有一个被修改了,也必须有相应处理动作
```C++
m3 = m1 + m2;        // 记录：m3 是 m1和m2的总和。
m1 = m4;              // 现在：m3 应该是 m2 和“m1旧值”的总和。
```
这里我们需要保证m1的赋值动作不会改变m3。

由于必须存储数值间的相依关系，而且必须维护一些数据结构以存储数值、相依关系，或是两者的组合，此外还必须将赋值（assignment）、复制（copying）、加法（addition）等操作符加以重载

上述4个例子显示，lazy evaluation 在许多领域中都可能有用途：可避免非必要的对象复制，可区别 operator[]的读取和写动作，可避免非必要的数据库读取动作，可避免非必要的数值计算动作。如果你的计算绝对必要，lazy evaluation 甚至可能使程序更缓慢，并增加内存用量，因为程序除了必须做你原本希望避免的所有工作之外，还必须处理那些为了lazy evaluation 而设计的数据结构。只有当“你的软件被要求执行某些计算，而那些计算其实可以避免”的情况下，lazy evaluation 才有用处。
