分组和聚集操作采用$\gamma$表示，使用该操作符可以根据元组的一个或多个属性将关系的元组分组，然后可以对关系中其他列使用聚集操作符进行聚集操作。如果关系已经分组，那么聚集操作是对每个分组进行。SQL子句通过聚集操作和特定的`GROUP BY`子句提供了$\gamma$的所有功能。SQL支持`SUM, COUNT, AVG, MIN, MAX`五个聚集操作。

当元组为空时，需要记录下面几条规则：
1. 空值在任何聚集操作中都被忽略。`COUNT(*)`是某个关系中所有元组数目之和，但`COUNT(A)`是对A属性非空的元组个数之和。
2. 在构成分组时，NULL值被当作一般的值对待，即分组中的一个或多个分组属性可以被赋予NULL值。
3. 除计数外，对空包执行的聚集操作结果均为NULL。空包的计数为0；

关于`HAVING`子句的规则：
1. `HAVING`子句中的聚集只应用到正在检查的分组上
2. 所有`FROM`子句中管子属性都可以出现在`HAVING`子句中用聚集计算，单只有出现在`GROUP BY`子句中的属性才可以以不聚集的方式出现在`HAVING`子句中。



SQL中允许指定是否对数据去重，例如`SUM(DISTINCT a)`表示去重后进行`SUM`聚集计算，`SUM(ALL a)`表示不去重进行聚集计算，`SUM(a)`等价于`SUM(ALL a)`。

除了`COUNT(*)`对关系中所有的元组技术，其他的句句函数都忽略null值，如果想要对属性a中$a != NULL$元组数计数，常用$count^{NN}(a)$在SQL中也就是`COUNT(a)`。

记x是一单行数值，$\{x\}$是行x的包，那么有：
$$
\begin{array}{l}
min(\{x\}) = x \\
max(\{x\}) = x \\
sum(\{x\}) = x \\
avg(\{x\}) = x \\
\end{array}
$$
可见，这些聚集函数如果运用到只含一个值的包上，和运用到单个值上是一样的。
$$
\begin{array}{c}
min(min(X)) = min(X)\\
max(max(X)) = max(X)\\
sum(sum(X)) = sum(X)\\
avg(avg(X)) = avg(X)\\
\end{array}
$$
也就是说，这些聚集函数是幂等的。



记$\mathcal{N} $表示某一数字数据类型(numeral data type)，例如integer或float，或元组$[a_1:\tau_1, ..., a_n:\tau_n]$其中$\tau$是数字数据类型，允许$\mathcal{N} $含NULL

Scalar聚集函数agg是一个带签名函数
$$
agg : \{\tau \}_b \to \mathcal{N}
$$
$agg : \{\tau \}_b \to \mathcal{N}$可以分解
$$
\begin{array}{l}
agg^1:\{\tau \}_b \to \mathcal{N}' \\
agg^2:\{\mathcal{N}' \}_b \to \mathcal{N} \\
\end{array}
$$
对于$Z = X \cup Y$，$X$和$Y$非空，
$$
agg{Z} = agg^2(\{agg^1(X), agg^1(Y)\}_b)
$$
$agg(Z)$可以在Z的任意subsets(-lists， -bags)上独立计算并且可以对部分结果聚集，从而得到正确的总结果。如果条件成立，可以说agg可以分解为inner $agg^1$和outer $agg^2$



如果对于$agg^O$存在函数$(agg^O)^{-1} : \mathcal{N}', \mathcal{N}' \to \mathcal{N} $满足：
$$
agg(X) = \gamma((agg^O)^{-1}(agg^I(Z), agg^I(Y)))
$$
那么称可分解聚集函数$agg : \{\tau \}_b \to \mathcal{N}$是可逆的。


$$
\begin{array}{l}
min(X \cup Y) &=& min(min(X), min(Y)) \\
max(X \cup Y) &=& max(max(X), max(Y)) \\
count(X \cup Y) &=& sum(count(X), count(Y)) \\
sum(X \cup Y) &=&sum(sum(X), sum(Y)) \\
\end{array}
$$

$$
\begin{array}{l}
avg(X \cup Y) &=& sum(sum(X), sum(Y))/ (count^{NN}(X) + count^{NN}(Y))
\end{array}
$$


`Agg`函数分解总结

|   $agg$    |      $agg^1$      | $agg^2$  |
| :--------: | :---------------: | :------: |
|    min     |        min        |   min    |
|    max     |        max        |   max    |
| `count(*)` |    `count(*)`     |   sum    |
|  count(a)  |     count(a)      |   sum    |
|    sum     |        sum        |   sum    |
|    avg     | $sum, count^{NN}$ | sum, sum |



如果$F$被分解为$F^1$和$F^2$，$F^1$又被分解为$F^{1，1}$和$F^{1,2}$，$F^2$被分解为$F^{2,1}$和$F^{2,2}$，那么有
$$
\begin{array}{l}
F^{1,1} = F^1 \\
F^{1,2} = F^2 \\
F^{2,1} = F^2 \\
F^{2,2} = F^2 \\
\end{array}
$$
