```
http://portal.acm.org/citation.cfm?id=362692&dl=ACM&coll=portal">Space/Time Trade-Offs in Hash Coding with Allowable Errors
```

在BloomFilter中关于误判概率的讨论的比较多的是m、n和k的影响，在看Hadoop中BloomFilter在注释中看到了这个链接，它关注的是在允许的误差范围内空间和时间的权衡。



# Hash函数

“散列(Hashing)” 的基本思想是：

- (1) 以关键字key为自变量，通过一个确定的函数$h$（散列函数）计算出对应的函数值$h(key)$，作为数据对象的存储地址。涉及**散列函数构造**。
- (2) 可能不同的关键字会映射到同一个散列地址上，即$h(key_i ) = h(key_j)$（当$key_i \ne key_j$），称为“冲突(Collision)”。此时需要**冲突解决策略**。



装填因子（Loading Factor）：设散列表空间大小为m，填入表中元素个数是n，则称α＝ n / m为散列表的装填因子



## 散列函数构造
字符关键词的散列函数构造，可以采用下面的方式：
- ASCII码加和法，简单的散列函数
对字符型关键词key定义散列函数
$h(key) = (\sum key[i]) \mod TableSize$

- 简单的改进——前3个字符移位法
$h(key) = (key[0] * 27^2 +  key[1] * 27 + key[2]) \mod TableSize$

- 移位法-好的散列函数
涉及关键词所有n个字符，并且分布得很好
$h(key) = (\sum_{i=0}^{n-1} key[n-i-1]*32^i) \mod TableSize$
```C++
Index Hash ( const char *Key, int TableSize ) { 
	unsigned int h = 0; /* 散列函数值，初始化为0 */
	while ( *Key != ‘\0’) /* 位移映射 */
 	h = ( h << 5 ) + *Key++;
 	return h % TableSize;
}
```
### 直接定址法

取关键词的某个线性函数值为散列地址，即$h(key) = a * key + b (a、b为常数)$



### 除留余数法

散列函数为：$h(key) = key \mod p$， 一般，p取素数



### 数字分析法

分析数字关键字在各位上的变化情况，取比较随机的位作为散列地址。



### 折叠法

把关键词分割成位数相同的几个部分，然后叠加。

```
例如，56793542

	542
 	793
+ 	056
———------
	1391
```



### 平方取中法

> 例如，56793542，h(56793542) = 641
>  	56793542
>  x 	56793542
> —————————--------
> 322550**641**2905764





## 冲突解决策略

常用处理冲突的思路：

- 开放地址法：换个位置
- 链地址法：同一位置的冲突对象组织在一起


### 开放定址法(Open Addressing)

一旦产生了冲突该地址已有其它元素），就按某种规则去寻找另一空地址。 

- 若发生了第 i 次冲突，试探的下一个地址将增加$d_i$，基本公式是$\begin{align}
    h_i(key) & = (h(key)+d_i) \mod TableSize & (1 \le i \lt TableSize)
    \end{align}$

- $d_i$ 决定了不同的解决冲突方案：
    - 线性探测 $d_i=i$
    - 平方探测 $d_i=\pm i^2$
    - 双散列 $d_i=i*h_2(key)$



 当散列表元素太多（即装填因子 α太大）时，查找效率会下降；

- 实用最大装填因子一般取$0.5 \le \alpha  \le 0.85$
- 当装填因子过大时，解决的方法是加倍扩大散列表，这个过程叫做“再散列（Rehashing）”



#### 线性探测法

Linear Probing，以增量序列 1，2，……，（TableSize -1)循环试探下一个存储地址。



#### 平方探测法

Quadratic Probing，也是二次探测。以增量序列$1^2$，-$1^2$，$2^2$，-$2^2$，……，$q^2$，-$q^2$ 且$q \le \left \lfloor TableSize/2 \right \rfloor $循环试探下一个存储地址

平方探测法有可能会存在有空间，但是平方探测找不到的情况。如下例$h(k)=k \mod 5$

| 5    | 6    | 7    |      |      |
| ---- | ---- | ---- | ---- | ---- |
| 0    | 1    | 2    | 3    | 4    |

插入11，$h(11)=1$

探测序列$1+1=2, 1-1=0, (1+2^2)\mod 5=0, (1-2^2)\mod 5=2, (1+3^2)\mod 5=0, (1-3^2)\mod 5=2, (1+4^2)\mod 5=2,…$



**【有定理显示】**：如果散列表长度TableSize是某个$4k+3 (k是正整
数)$形式的素数时，平方探测法就可以探查到整个散列表空间。



#### 双散列探测法

Double Hashing，$d_i 为i*h_2(key)，h_2(key)是另一个散列函数$， 探测序列成：$h_2(key)，2h_2(key)，3h_2(key)，……$

- 对任意的key，$h_2(key) \ne 0$
- 探测序列还应该保证所有的散列存储单元都应该能够被探测到。选择以下形式有良好的效果：$h_2(key) = p - (key \mod p)$
    其中：$p \lt TableSize$，p、TableSize都是素数。



**Note:** 在开放地址散列表中，删除操作要很小心。通常只能“懒惰删除”，即需要增加一个“删除标记(Deleted)”，而并不是真正删除它。以便查找时不会“断链”。其空间可以在下次插入时重用。



### 分离链接法

Separate Chaining，将相应位置上冲突的所有关键词存储在同一个单链表中。





## 散列表查找性能分析

- 成功平均查找长度(ASLs)：查找表中关键词的平均查找比较次数（其冲突次数加1）
  
- 不成功平均查找长度 (ASLu)：不在散列表中的关键词的平均查找次数（不成功）



## 练习题

- https://leetcode.com/problems/valid-anagram/description/

- https://leetcode.com/problems/two-sum/description/

-  https://leetcode.com/problems/3sum/description/

- https://leetcode.com/problems/4sum/

- https://leetcode.com/problems/group-anagrams/description/



