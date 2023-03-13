

# Set, Bag,Sequence

## 集合(Sets)

特征函数(characteristic function)为$\chi_S(s)$
$$
\begin{eqnarray}
\chi_S(s) = \begin{cases}
  & 0  & if \ s \notin S \\
  & 1 & if \ s \in S
\end{cases}
\end{eqnarray}
$$
包S的基数(cardinality )$\left | S \right |$的定义为
$$
\left | S \right | = {\textstyle \sum_{x}\chi_S(x)}
$$


### 集合操作的定律



## 包(Bag)

特征函数(characteristic function):$\chi_B(x)$，表示元素$x$在包B中的出现次数。包B 的基数(cardinality )$\left | B \right |$的定义为
$$
\left | B \right | = {\textstyle \sum_{x}\chi_B(x)}
$$
对于一个包B和特征函数，有$x \in B \prec \succ \chi_B(x) > 0$



两个包的并$ X \cup_b Y$的定义是并集中元素出现的次数等于该元素在$X$和$Y$中出现的次数之和，包的交集$X \cap_b Y$是交集中的元素出现的次数是该元素在$X$和$Y$中出现的次数的最小值，包的差$X \setminus_b Y$是交集中的元素出现的次数是该元素在$X$和$Y$中出现的次数的差($\dotminus$)，其中$a \dotminus b$定义为$max(0, a-b)$。运用包的特征函数，可以有如下的定义。

$$
\begin{array}{l}
\chi_{X \ \cup_b \  Y}(z) = \chi_{X}(z) + \chi_{Y}(z) \\
\chi_{X \ \cap_b \ Y}(z) = min(\chi_{X}(z), \chi_{Y}(z)) \\
\chi_{X \ \setminus_b \  Y}(z) = \chi_{X}(z) \dotminus \chi_{Y}(z)
\end{array}
$$


下面证明$X \cup_b (Y \cap_b Z) = (X \cup_b Y) \cap_b (X \cup_b z)$,对于所有的$x$有
$$
\begin{eqnarray}
\chi_{LHS}(x) & = & \chi_{X}(x) + min(\chi_{Y}(x), \chi_{Z}(x)) \\ 
	& = & min(\chi_{X}(x) + \chi_{Y}(x), \chi_{X}(x) + \chi_{Z}(x)) \\ 
	& = & \chi_{RHS}(x)
\end{eqnarray}
$$

证明$(X \cap_b Y) \setminus_b Z = (X \setminus _b Y) \cap_b (X \setminus_b z)$，对于所有的$x$有
$$
\begin{eqnarray}
\chi_{LHS}(x) & = & min(\chi_{X}(x) , \chi_{Y}(x)) \dotminus  \chi_{Z}(x) \\ 
& = & min(\chi_{X}(x) \dotminus  \chi_{Z}(x), \chi_{Y}(x) \dotminus \chi_{Z}(x)) \\
& = & \chi_{RHS}(x)
\end{eqnarray}
$$

## 显式重复控制
每一个集合都可以看做特征函数不超过1的包。采用$\bar{I}(S)$表示将集合S转化为采用相同特征函数的包，采用$\bar{I}^{-1}(B)$表示如果包的特征函数不超过1，将包转化为集合。否则，$\bar{I}^{-1}$未定义。
$$
\bar{I}^{-1}(\bar{I}(X) \cap_b \bar{I}(Y)) = X \cap_s Y
$$
也就是说，两个集合$X$和$Y$的包交和集合交是一样的。因此，只需要一个交操作，将其定义在包上，采用$\cap$来定义。

由此引出了set-faithfulness的概念，称集合上一元函数$f$是set-faithful，当且仅当
$$
\bar{I}^{-1}(f(\bar{I}(X))) = f(X)
$$

依次类推，我们称二元函数$g$是set-faithful,当且仅当
$$
\begin{array}{l}
\bar{I}^{-1}(g(\bar{I}(Y), \bar{I}(Y))) = g(X, Y)
\end{array}
$$

$\setminus_{b}$和$\cap_{b}$是set-faithful，因此可以简单地使用$\setminus$和$\cap$来表示差(difference)和交(intersection)。$\cup_b$是非set-faithful，需要特别注意$\cup_s$和$\cap_b$之间的差别。


从包到集合，必须去重复，采用$\Pi^D$表示重复操作，对于包B，那么有$\chi_{\Pi^D(B)}(z) = min(1, \chi_B(z))$。可以采用$\Pi^D$将$\cup^d$定义为:
$$
R_1 \cup^d R_2 \coloneq = \Pi^D(R_1 \cup R_2)
$$

## 有序数据:Sequences
Sequence是有序且可能重复，例如$\left \langle a, b, b, c, d \right \rangle$是序列。$|S|$表示序列的长度，它表示序列中元素的数量，示例的序列长度为5。$\epsilon$表示一个空序列，含有0个元素。

长度$n \ge 0$的序列有特征函数$\chi$。记S为序列，那么$\alpha(S)$表示序列第一个元素，S剩余的记为$\tau(S)$, 它包含除第一个元素意外的其他所有元组，也就是说$\chi_{\tau(S)}(i) = \chi_S(i+1)$

对于是示例$\left \langle a, b, b, c, d \right \rangle$有
$$
\alpha(S) = a \\
\tau(S) = \left \langle b, b, c, d \right \rangle
$$

两个Sequence的连接定义为$\oplus$，两个Sequence S和T的特征函数表示为：
$$
\begin{array}{l}
\chi_{S \oplus T}{i} = \begin{cases}
  & \chi_S(i) & \text{ if } i < |S| \\
  & \chi_T(i - |S|) & \text{ if }  i \ge |S|
\end{cases}
\end{array}
$$
例如:$\left \langle a, b, b, c, d \right \rangle  \oplus \left \langle a, b, c \right \rangle = \left \langle  a, b, b, c, b, a, b, c\right \rangle$

类似于集合和包上的线性，引入sequence上一元和n元函数的序列线性(sequence linearity)。对于一个一元函数从序列到序列称为序列现象(sequence-linear)，当且仅当下面两个条件对于所有的X和Y都成立：
$$
\begin{array}{l}
f(\epsilon ) & = & \epsilon \\
f(X \oplus Y) &= & f(X) \oplus f(Y)
\end{array}
$$

一个从多个序列到一个序列的映射称为*sequence-linear in its i-th argument i*。当且仅当，对于所有的序列$X_1, ..., X_n, {X-i}'$都成立
$$
\begin{array}{l}
f(X_1, ..., X_{i-1}, \epsilon,X_{i+1}, ..., X_n ) & = & \epsilon \\
f(X_1, ..., X_{i-1}, {X_i} \oplus {X_i}', X_{i+1}, ..., X_n) & = & f(X_1, ..., X_{i-1}, {X_i}, X_{i+1}, ..., X_n) \oplus f(X_1, ..., X_{i-1}, {X_i}',X_{i+1}, ..., X_n )
\end{array}
$$

如果对于所有的参数都是sequence-linear，则称其为sequence-linear。二元函数或操作，我们可以区分左参数和右参数，如果它的左(右)参数是sequence-linear，那么，称left(right) sequence-linear。



# 聚集函数(Aggregation Functions)



# 算子(Operator)

## 投影(Project)

记$A\{ a_1, ..., a_n\}$为一组属性的集合，有两种Project算子的定义
$$
\begin{array}{l}
\Pi_A(e) \coloneq \{ \left [  a_1:x.a_1, ... , a_n: x.a_n\right ] | x \in e \}_b,\\
{\textstyle \Pi_{A}^{D}} \coloneq \Pi^D(\Pi_A(e))
\end{array}
$$


