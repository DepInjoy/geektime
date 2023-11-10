```scala
// _是通配符,类似于Java中的*
import scala.math._

// 包以scala.开头时,可以省去scala前缀
// 等同于import scala.math._
import math._
```

Scala没有静态方法，它支持类似特性叫单例对象(Singleton Object). 通常，一个类对应有一个伴生对象(Companion Object)和Java的静态方法类似。

不带参数的Scala方法通常不使用`()`。一般，无参数且不改变当前对象的方法不带`()`。