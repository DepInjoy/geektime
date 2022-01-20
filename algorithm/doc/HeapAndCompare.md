# 堆和比较器



## 比较器

比较器，实质就是重载比较运算符

1. 比较器可以很好的应用在特殊标准的排序上
2. 比较器可以很好的应用在根据特殊标准排序的结构上
3. 容易实现，还可用于范型编程



### C++比较器

C++中定义了一些[比较运算符Comparison Operators](https://en.cppreference.com/w/cpp/language/operator_comparison),在编码实现上实现的套路是类似的，堆可以分为大顶堆和小顶堆，在此重点关注<运算符以及重载。

```C++
/**
 * @brief   类型T实现<运算符
 */
bool T::operator <(const T2 &b) const;

/**
 * @brief   非成员函数,实现<运算符
 * 
 * @return  如果lhs小于rhs返回true,否则返回false
 */
bool operator <(const T &a, const T2 &b);
```

在代码实现上主要有三种实现方式：

1.  对自定义的`struct/class`，重写它的`operator <`方法。
2.  通过写一个外部的比较函数，实现`<`方法，适合于
    1.  比较内置数据类型，例如对`string`按照长度进行排序。
    2.  无法修改需要比较的类型。
    3.  除类型自定义的比较方式以外的比较方法。

3. 通过函数对象结构体来实现`<`方法



自定义了一个`student`结构体按照上述方式实现了<运算符，代码详见[自定义比较器](https://github.com/DepInjoy/geektime/blob/main/algorithm/CPP/Comparator/CustomComparator.cpp)

### Java比较器

Java可以通过两种方式来实现比较器，一种是继承`Comparable`，并重写`compareTo`接口.

```java
public interface Comparable<T> {
    /**
     * @param   o the object to be compared.
     * @return  a negative integer, zero, or a positive integer as this object
     *          is less than, equal to, or greater than the specified object.
     *
     * @throws NullPointerException if the specified object is null
     * @throws ClassCastException if the specified object's type prevents it
     *         from being compared to this object.
     */
    public int compareTo(T o);
}
```

另外一种是借助`Comparator`函数式编程接口来实现

```java
@FunctionalInterface
public interface Comparator<T> {
    int compare(T o1, T o2);
}
```



自定义了一个`student`结构，实现按照首先按照ID升序再按照Age降序，最后按照Name升序进行排序，代码详见[自定义Java比较器](https://github.com/DepInjoy/geektime/blob/main/algorithm/Java/algorithm-project/src/main/java/algorithm/training/struct/Heap/C06_01_Comparator.java)

