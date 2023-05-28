旧式的C转型方式几乎允许你将任何类型转换为任何其他类型。如果每次转型都能够更精确地指明意图，则更好。举个例子，将一个pointer-to-const-object转型为一个 pointer-to-non-const-object （也就是说只改变对象的常量性），和将一个 pointer-to-base-class-object 转型为一个 pointer-to-derived-class-object（也就是完全改变了一个对象的类型），其间有很大的差异。传统的 C转型动作对此并无区分(因为C转型动作是为C设计的并非C++)。

为解决C旧式转型的缺点，C++导入4个新的转型操作符(cast operators)：
1. `const_cast`:改变表达式中的常量性（constness)或变易性(volatileness)
2. `dynamic_cast`:将继承体系中安全的向下转型，`dynamic_cast`可以将base class objects的 pointers或references”转型为“指向derived(或 sibling base) class objects的pointers或references”
3. `static_cast`：在无不涉及继承机制的类型执行转型动作。
4. `reinterpret_cast`：常用于函数指针类型转换。

```C++
typedef void (*FuncPtr)();
FuncPtr funcPtrArray[10];

int doSomething();

// 用reinterpret_cast强制进行函数指针转型
funcPtrArray[0] = reinterpret_cast<FuncPtr>(&doSomething);
```

如果你的编译器尚未支持这些新式转型动作，你可以使用传统转型方式来取代static_cast，const_cast 和 reinterpret_cast。甚至可以利用宏来仿真这些新语法
```C++
#define static_cast(TYPE, EXPR) (TYPE)(EXPR)
#define const_cast(TYPE, EXPR) (TYPE)(EXPR)
#define reinterpret_cast(TYPE, EXPR) (TYPE)(EXPR)
```