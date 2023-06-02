关系演算以数理逻辑中的谓词演算为基础，根据谓词变量的不同，可以分为关系元组演算和关系域演算：

- 关系元组演算是以元组变量作为谓词变量的基本对象。
- 关系域演算是以域变量作为谓词变量的基本对象。

# 关系元组演算

关系元组演算公式的基本形式:
$$
\begin{array}{l}
 \{t | P(t)\} \\
上式表示:所有使谓词P为真的元组t的集合 \\
\\
t是元组变量 \\
t \in r 表示元组t在关系r中\\
t[A]表示元组t的分量，即t在属性A上的值 \\
P是与谓词逻辑相似的公式，P(t)表示以元组t为变量的公式\\
\end{array}
$$
$P(t)$可以是如下三种形式之一的**原子公式**：

- $t\in R$，t是关系R中的一个元组。
- $s[A] \ \theta \ c$，元组分量$s[A]$与常量c之间满足比较关系$\theta$。$\theta$：比较运算符$<, \le, =, <>, >, \ge$
- $s[A] \ \theta \ u[B]$，$s[A]$与$u[B]$为元组分量，A和B分别是某些关系的属性，它们之间满足比较关系$\theta$.

$P(t)$可以由公式加运算符$\wedge(与), \vee(或)， \neg(非)$递归地构造

- 如果F是一个公式，则$\neg$也是公式
- 如果$F_1$、$F_2$是公式，则$F_1 \wedge F_2$，$F_1 \vee F_2$也是公式。

构造$P(t)$还有两个运算符:$\exists(存在), \forall(任意)$

- 如果F是一个公式，则$\exists(t\in r)(F(t))$也是公式
- 如果F是一个公式，则$\forall(t\in r)(F(t))$也是公式

运算符$\exists$和$\forall$又称为量词，前者称存在量词，后者称全称量词。被$\exists$和$\forall$限定的元组变量t，或者说元组变量t前有存在量词或全称量词，则该变量被称为**约束变量**，否则被称为**自由变量**。

$\exists(t\in r)(F(t))$ 表示$t \in r$对r中的每个t进行F(t)检验:

- 所有t，都使$F(t)$为假，则结果为假。
- 有一个t，使得$F(t)$为真，则结果为真。

$\forall (t\in r)(F(t))$表示$t \in r$对r中的每个t进行F(t)检验

- 所有t，都使$F(t)$为真，则结果为真。
- 有一个t，使得$F(t)$为假，则结果为假。

上述运算符的优先级次序由高到底为括弧、$\theta$、$\exists$、$\forall$, $\neg$、$\wedge$，$\vee$

## 元组演算等价性变换

 $\neg$、$\wedge$，$\vee$运算间的等价性
$$
\begin{array}{l}
P1 \wedge P2 \Longleftrightarrow \neg(\neg P1 \vee P2 ) \\
n个否定的或操作的再否定，便是n个肯定的与操作 \\

\\
P1 \vee P2 \Longleftrightarrow \neg(\neg P1 \wedge \neg P2) \\
n个否定的与操作的再否定，便是n个肯定的或操作\\
\end{array}
$$
$\exists$、$\forall$, $\neg$运算间的等价
$$
\forall(t \in R)(P(t)) \Longleftrightarrow \neg(\exists(t \in R)(\neg P(t))) \\
\exists(t \in R)(P(t)) \Longleftrightarrow \neg(\forall(t \in R)(\neg P(t)))
$$


## 元组演算和关系代数的等价性

$$
\begin{array}{l}
并运算：R \cup S = \{ t | t \in R \vee t \in S \} \\
差运算：R - S = \{ t | t \in R \vee \neg(t \in S) \} \\
交运算：R \cap S = \{ t | t \in R \wedge  t \in S \} \\
\\
广义笛卡尔积：R(A) \times S(B) = \{ t | \exists(u \in R) \exists(s \in S(t[A]=u[A] \wedge t[B]=S[B])) \} \\
\\
\sigma_{con}(R) = \{ t | t \in R \wedge F(con) \} \\
\pi_A(R) = \{ t[A] | t \in R \} \\
\end{array}
$$



# 关系域演算

