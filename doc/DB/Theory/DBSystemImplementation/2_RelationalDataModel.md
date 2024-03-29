# 关系数据库模型

## 1. 数据模型概述

## 2. 关系模型基础

## 3. SQL中定义关系模式

## 4. 代数查询语言
### 关系代数
传统关系代数的操作主要有以下四类：
1.通常的关系操作:并、交、差。
2. 除去某些行或列的操作：选择是消除某些行(元组)的操作，而投影是消除某些列的操作。
3. 组合两个关系元组的操作。包括笛卡尔积运算(Cartesian Product),该操作尝试两个关系所有可能的元组配对方式形成一个关系作为结果，另外还有很多连接操作，它从两个关系中选择一些元组配对。
4. 重命名操作(Renaming)。不影响关系中的元组，但是改变了关系模式，即属性的名称或关系本身名称。

一般将关系代数的表达式称为查询(Query)。

#### 关系上的集合操作

#### 投影

#### 选择

#### 笛卡尔积

#### 自然连接

#### $\theta$连接

#### 组合操作

#### 命名和重命名

### 操作间的联系

# 
$$
\begin{array}{l}
\Pi_{F_1, F_2, ......, F_n}(E) \\
\end{array}
$$
- $E$ is any relational-algebra expression
- Each of$F_1, F_2, ......, F_n$are arithmetic expressions involving constants and attributes in the schema of $E$.

Aggregation function takes a collection of values and returns a single value as a result.
- avg: average value
- min: minimum value
- max: maximum value
- sum: sum of values
- count: number of values

Aggregate operation in relational algebra. 
$$
\begin{array}{l}
_{G_1, G_2, ..., G_n}\mathcal{G}_{F_1(A_1), F_2(A_2, ......,F_n(A_n))}
\end{array}
$$
- $E$ is any relational-algebra expression
- $G_1, G_2, ..., G_n$is a list of attributes on which to group (can be empty)
- Each$F_i$ is an aggregate function
- Each $A_i$ is an attribute name

# 参考资料
- [Relational algebra(关系代数)](https://www.cbcb.umd.edu/confcour/Spring2014/CMSC424/Relational_algebra.pdf)