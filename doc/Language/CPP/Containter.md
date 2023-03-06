# 顺序容器

顺序容器(sequential container)为程序员提供了控制元素存储和访问顺序的能力。这种顺序不依赖于元素的值，而是与元素加入容器时的位置相对应。

| 容器         | 特点                                                 |
| ------------ | ---------------------------------------------------- |
| vector       | 可变大小数据，支持随机访问。                         |
| deque        | 双端队列，支持随机访问。                             |
| list         | 双向链表，只支持双向顺序访问                         |
| forward_list | 单向链表，只支持单向顺序访问。                       |
| array        | 固定大小数组，支持快速随机访问，不能添加或删除元素。 |
| string       | 与vector类似，专门用于保存字符，支持随机访问。       |



# 关联容器

按照关键字有序保存的元素的关联容器

| 容器     | 描述                             |
| -------- | -------------------------------- |
| map      | 关联数组，保存key-value键值对    |
| set      | 关键字即值，即只保存关键字的容器 |
| multimap | 关键字可以重复出现的map          |
| multiset | 关键字可以重复出现的set          |

无序集合

| 容器               | 描述                             |
| ------------------ | -------------------------------- |
| unordered_map      | 用hash函数组织的map              |
| unordered_set      | 用hash函数组织的set              |
| unordered_multimap | Hash组织的map:关键字可以重复出现 |
| unordered_multiset | Hash组织的set:关键字可以重复出现 |



# bitset

`std::bitset`构造出一个内含bit或Boolean值且大小固定的array. C程序和传统C++程序通常使用类型long来作为bit array，再借由&、|、~等位操作符操作各个bit. `std::bitset`可以容纳任意个数的bit，并提供`&, |, ^, ~, &=, |=, ^=`等操作，但它不可以动态修改个数，如果需要一个可变长度的bit容器，可考虑使用`vector<bool>`。
```C++
// Since C++11, bitset的Hash支持
std::hash<std::bitset>
```

## 常用操作
```C++
void BitsetAPIExample() {
    // 1. 构造
    std::bitset<16> bset1;
    std::bitset<16> bset2(3);
    std::bitset<16> bset3("1011111011");
    std::cout << "1: " << bset1 << " 2: " << bset2 << " 3: " << bset3 << std::endl;

    // 2. 位运算
    std::bitset<8> bset4(8);
    bset4[0] = 1, bset4[1] = 1;
    std::cout << "After change 0 and 1 bit " << bset4 << std::endl;
    bset4 |= 0b10000;
    std::cout << "After change |= 5 bit " << bset4 << std::endl;
    bset4 &= ~0b10000;
    std::cout << "After change &= 5 bit " << bset4 << std::endl;
    bset4.set(4, 10);
    std::cout << "After change set 5 bit as 1 " << bset4 << std::endl;

    // 3. reset清零/flip位取反
    bset4.reset();
    std::cout << "After reset " << bset4 << std::endl;
    bset4.flip(0);
    std::cout << "After flip " << bset4 << std::endl;

    // 4. 转换为和string
    std::bitset<8> bset5(255);
    std::cout << bset5.to_ullong() << " " << bset5.to_string() << std::endl;

    // 5. count/all/any/none接口
    //      count: bit位数
    //      all 所有位都为1返回true,否则false
    //      any 存在1位为1返回true,否则false
    //      none 所有位都为0返回true,否则false
    std::cout << "count: " << bset1.count() << std::endl;
    std::cout << "all: " << bset1.all() << " any:" << bset1.any() << " none:"<< bset1.none() << std::endl;
    std::cout << "all: " << bset4.all() << " any:" << bset4.any() << " none:"<< bset4.none() << std::endl;
    std::cout << "all: " << bset5.all() << " any:" << bset5.any() << " none:"<< bset5.none() << std::endl;
}
```
