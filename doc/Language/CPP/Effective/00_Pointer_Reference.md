
**引用(reference)一定得代表某个对象，C++因此要求引用必须有初值，没有null reference。如果有一个变量，用来指向一个对象，但也可能不指向人和对象(null指针)，那么应该采用指针。**
```C++
std::string s("xyz");
// error,引用必须被初始化
std::string& rs;
// 将rs指向s
std::string& rs = s;
```

```C++
// 指针可以不初始化，有效但风险高
std::string* s;
```
由于没有null reference也就以为这不需要判断其有效性，因此引用比指针效率更高。
```C++
voif PrintDouble(const double& rd) {
    // 不需要检测有效性，它一定指向某个double
    std::cout << rd;
}

voif PrintDouble(const double* pd) {
    // 需要检查指针是否为null
    if (pd) {
        std::cout << *pd;
    }
}
```
---


**指针和引用间的另一个重要差异是指针可以被重新赋值，指向另一个对象，引用却总是指向它最初获得的那个对象。**一般而言，如果需要考虑将对象不指向任何对象(将指针设置为null)，或者在不同时间指向不同的对象(改变指针指向的对象)，就应该使用指针。当确定相关对象总是代表一个对象且一旦代表了该对象就不能改变，那么应该使用引用。

```C++
std::string s1("Nancy");
std::string s2("Clancy");

// rs指向s1
std::string& rs = s1;
// ps指向s1
std::string* ps = &s1;

// rs依然指向s1,但s1的值变成了Clancy
rs = s2;

// ps指向s2,s1没有发生改变
ps = &s2;
```
---

还有其他情况也需要使用引用，例如实现某些操作符时。最常见的例子就是`operator[]`。
```C++
std::vector<int> v(10);
// 对operator[]返回的对象赋值
v[5] = 10;
```
如果`operator[]`返回的事指针，那么赋值语句必须写成
```C++
*v[5] = 10;
```
这使得`v`看起来是以指针形式形成的`std::vector`,而事实上并不是。

# 参考资料
1. More Effective C++