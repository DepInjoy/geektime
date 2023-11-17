匿名内部类存在的一个问题是，如果匿名内部类的实现非常简单，例如只包含一个抽象方法的接口，那么匿名内部类的语法仍然显得比较冗余。为此，JDK 8中新增了一个特性Lambda表达式，这种表达式只针对有一个抽象方法的接口实现，以简洁的表达式形式实现接口功能来作为方法参数。

Lambda表达式由三个部分组成，分别为参数列表、->和表达式主体，其语法格式如下：
```java
 ([数据类型 参数名, 数据类型 参数名,...]) -> {
   表达式主体
}
```
[Lambda表达式实现示例](ProgramLanguage/Java/src/main/java/base/lambda/LambdaExample.java)

# 函数式接口
Java中，函数式接口指有且仅有一个抽象方法的接口，而Lambda表达式是基于函数式接口实现的。Lambda表达式是Java中函数式编程的体现，只有确保接口中有且仅有一个抽象方法, Lambda表达式才能顺利地推导出实现的这个接口中的方法。

```java
 @FunctionalInterface
 修饰符 interface 接口名称 {
    返回值类型 方法名称(可选参数信息);
    // 其他非抽象方法内容
 }
```
`@FunctionalInterface`是非必需的, 在JDK 8中，专门为函数式接口引入了一个`@FunctionalInterface`注解，该注解只是显示的标注了接口是一个函数式接口，并强制编辑器进行更严格的检查，确保该接口是函数式接口，如果不是函数式接口，那么编译器就会报错，而对程序运行并没有实质上的影响

# 构造器引用和方法引用
[构造器引用实现示例](ProgramLanguage/Java/src/main/java/base/lambda/ConstructorReference.java)

# 参考资料
1. [Lambda表达式入门](https://book.itheima.net/course/1265899443273850881/1265900797362954241/1265905019340972033)
2. [方法引用-Java的::语法](https://blog.csdn.net/tgvincent/article/details/108534445)
