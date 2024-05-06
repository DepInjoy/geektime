
# 类型

**采用统一类型是 Scala 的语法特性之一。** 在数据类型方面 Scala 与 Java 的最大区别在于：**Scala 没有所谓的基本数据类型，秉承一切皆对象的彻底性，所有的数据类型和函数都是以对象的形式存在。** Scala并不刻意区分基本类型和引用类型，可以直接对数字执行方法：
```scala
1.toString() // 产生字符串"1"
```

<center>
    <img src="./img/scala_type_level.svg">
    <div>scala类型层次结构</div>
</center>

Any 是所有类型的超类,，是Scala类层级结构的跟，是一个抽象类，也称顶级类型，它定义了一些通用的方法如 `equals`、`hashCode` 和`toString`，Scala中的每个类都直接或间接从`Any`继承。Any 有两个之间的子类 `AnyValue`和`AnyRef`，它们都是类，Scalar不刻意区分基本类型和引用类型。

- `AnyVal`代表值类型，它有 9 个预定义的非空的值类型，分别是 Double、Float、Long、Int、Short、Byte、Char、Unit 和 Boolean。Unit是不带任何意义的类型，Unit 可以作为返回类型,，和C++中的void一样。

- `AnyRef`代表引用类型，所有非值类型都被定义为引用类型，用户声明的自定义类型都属于`AnyRef`引用类型的子类型，同时在 Java 运行环境中调用 Scala。`AnyRef`会被当做`java.lang.Object`基类。å

Nothing是所有类型(包括值类型和引用类型)的子类型，同时Nothing也是Null 的子类型，也称为底部类型。没有一个值是 Nothing 类型的，通常用于程序非正常结束的信号，可以被理解为不定义值的表达类型，在非正常返回时使用。这点与 Java 中返回 Null，C++中用-1 作为返回符类似。

Null 是所有引用类型的子类型(即`AnyRef`的子类型)，它有一个单例值由关键字 Null 所定义。Null 主要是使得 Scala 满足和其他 JVM 语言的互操作性，但是 null 是非常容易引发程序崩溃，同时 Scala 也采用了各种机制来避免使用 Null 类型。

Scalar还提供了`RichInt`，`RichDouble`，`RichChar`等，它们提供了`Int`，`Double`，`Char`等不具备的便捷方法。`BigInt`，`BigDecimal`可以用于任意大小(但有穷)的数字，它们背后对应`java.math.BigInteger`和`java.math.BigDecimal`。

## 元组

在Scala中，元组是一个可以容纳不同类型元素的类，元组是不可变的，可用于从函数返回多个值。

# 变量

scala变量可分为变量和常量

- 变量，指的是在程序运行过程中值可能会改变的量，用关键词`var`声明。
- 常量，指的是程序运行中值不会发生改变的量，使用关键词`val`声明常量，如果对常量修改在编译时会报错。

同时，scala有语法糖，编译器会自动根据变量和常量的初始值推断出类型。

```scala
// 变量，可以被修改
var myVar : String  = "Spark Streaming"
myVar = "Action"
println(myVar + ", " + myVal)

// 常量, 不可修改，若修改，编译报错
val myVal : String  = "Spark Streaming"

// 语法糖,age会自动推断出int类型
var age = 18
```
## 懒值(lazy)
```scala
lazy val words = scala.io.Source.fromFile("/path")
```

# 算术和操作符重载
Scalar的算术操作符和Java或C++一样，支持`+-*/%`等操作符以及位运算`&|^>> <<`，不同的地方在于这些操作符实际上也是方法，例如，`a + b`实际上是`a.+(b)`的简写。Scalar并没有提供++和--操作符。

# 控制结构

## 条件表达式

## 循环
Scala的没有提供break或continue来退出循环。如果需要退出循环可以采用：
1. 使用Boolen型的控制变量
2. 使用嵌套函数
3. 使用Breaks对象中的break方法
    ```scala
    import scala.util.control.Breaks._

    breakable {
        for (...) {
            if (...) break; // 退出breakable块
            ...
        }
    }
    ```

# 函数
```scala
// _是通配符,类似于Java中的*
import scala.math._

// 包以scala.开头时,可以省去scala前缀
// 等同于import scala.math._
import math._
```

Scala没有静态方法，它支持类似特性叫单例对象(Singleton Object). 通常，一个类对应有一个伴生对象(Companion Object)和Java的静态方法类似。

不带参数的Scala方法通常不使用`()`。一般，无参数且不改变当前对象的方法不带`()`。

递归函数必须指定返回值的类型
```scala
def fac(n : Int) : Int = if (n <= 0) 1 else n * fac(n-1)
```

# 异常
与Java不同，Scala不支持受检异常(不需要说明函数或方法可能会抛出某种异常)。


# 参考资料
1. Spark Streaming 实时流式大数据处理实战
2. 快学scala
