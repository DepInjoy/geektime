# 集合(Sets)

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


## 集合操作的定律



# 包(Bag)

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

# 投影(Project)

记$A\{ a_1, ..., a_n\}$为一组属性的集合，有两种Project算子的定义
$$
\begin{array}{l}
\Pi_A(e) \coloneq \{ \left [  a_1:x.a_1, ... , a_n: x.a_n\right ] | x \in e \}_b,\\
{\textstyle \Pi_{A}^{D}} \coloneq \Pi^D(\Pi_A(e))
\end{array}
$$


