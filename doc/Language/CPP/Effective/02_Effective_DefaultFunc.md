C++编译器会为empty class声明默认的dedault构造函数、Copy构造函数、copy assgnment操作符、析构函数，它们都是 public iniine，编译器产出的析构函数是个non-virtual（见条款7），除非这个class的基类自身声明有virtual析构函数实现：
```C++
class TimeKeeper {
public:
    TimeKeeper();
    // 虚析构函数，确保通过TimeKeeper*指针释放派生类资源
    // 派生对象的析构函数会被调用，来确保派生对象资源释放
    virtual ~TimeKeeper ();
    ...
};

class AtomicClock : public TimeKeeper {
    // ......
};

class WaterClock : public TimeKeeper {
    // ......
};

class WristClock : public TimeKeeper {
    // ......
};
```

在程序使用中可以添加一个设计工厂来实现获取记时对象
```C++
// 指向TimeKeeper派生的动态分配对象的指针
TimeKeeper* getTimeKeeper();
```

```C++
// 通过设计工厂函数获取记时对象
TimeKeeper* ptk = getTimeKeeper();  // 正确
...

// 使用完成，释放避免资源泄露
delete ptk;
```

<b><font color=FA8072>C++明确指出，当派生对象经由一个基类指针删除，而该基类带着一个non-virtual析构函数，其结果未有定义——实际执行时通常发生的是对象的派生成分没被销毁，通常基类的成分会被销毁，这会造成一个诡异的局部销毁对象，可能造成资源泄露。</font></b>因此，这里`delete ptk;`需要确保基类`TimeKeeper`的析构函数是虚函数。

如果class不含virtual函数，通常表示它并不打算被用做一个base class。当class不打算被当作基类，不应该将析构函数为声明为`virtual`。例如声明`Point`类
```C++
class Point {
public:
    Point(int xCoord, int yCoord);
    ~Point();;
private:
    int x,y;
};
```

<b><font color=FA8072>如果Point析构函数是virtual，对象必须携带某些信息，主要用来在运行期决定哪一个virtual函数该被调用，通常是由一个所谓vptr(virtual table pointer)指针指出。vptr指向一个由函数指针构成的数组，称为vbl(virtual table);每一个带有virtual函数的class都有一个相应的vbl。当对象调用某一virtual 函数，实际被调用的函数取决于该对象的vptr所指的那个vbtl——编译器在其中寻找适当的函数指针。</font></b>

如果Point class内含virual函数，其对象的体积会增加;在32-bit计算机体系结构中将占用64bits(为了存放两个ints)至96 bits(两个ints加上vptr); 在64-bit计算机体系结构中可能占用64~128 bits，因为指针在这样的计算机结构中占64bits。因此，为Point添加一个vptr会增加其对象大小达50%~100%，Point对象不再能够塞入一个64-bit缓存器，而C++的Point对象也不再和其他语言(如 C)内的相同声明有着一样的结构(因为其他语言的对应物并没有vptr)，因此也就不再可能把它传递至或接受自其他语言所写的函数，也因此不再具有移植性。

---

polymorphic(带多态性质的)基类应该声明一个`virtual`析构函数。如果class带有任何`virtual`函数，其析构函数就应该是`virtual`。Classes 的设计目的如果不是作为基类使用，或不是为了具备多态性polymophically，就不该声明`virtual`析构函数。

---