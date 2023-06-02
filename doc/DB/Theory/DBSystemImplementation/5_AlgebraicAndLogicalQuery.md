

代数和逻辑查询语言

# 代数查询语言



# 包上的关系操作

## 包的并、交、差

假设R和S是包，其中元组t在R中出现了n次，在S中出现了m次，其中m和n可以是0，于是：

- 在$R \cup S$的包并操作中，元组t出现n+m次
- 在$R \cap S$的包交操作中，元组t出现min(n,m)次
- 在$R-S$的包差操作中，元组t出现，max(0, n-m)次，也就是说，如果元组t在R中出现的次数比在S中出现的次数多，则$R-S$中t出现的次数就是将t在R中出现的次数减去t在S中出现的次数，如果元组t在R中出现的次数比在S中出现的次数一样多，那么t在$R-S$中就不出现了。直观上，t在S中每次出现都“抵消”它在R中一次出现。


## 关系代数的扩展操作符

# 关系逻辑

逻辑查询语言Datalog(database logic)由if-then规则组成，这些规则表示：从某个关系的特定元组的组合可以推断出另外一些元组必定满足另一关系，或满足查询结果。



**谓词和关系**

关系在Datalog中由谓词(predicate)表示，。每个谓词拥有固定数目的参数，一个谓词和它的参数一起被称为原子(atom)，为了和算术原子区分，也称为关系原子(relation atom)。原子的语法像变成语言中函数调用语法。例如$P(x_1, x_2,....x_n)$即是一个由谓词P和参数$x_1, x_2,..., x_n$组成的原子。实质上谓词就是一个返回布尔值的函数名。如果R是一个包含n个固定顺序的属性关系，那么可以用R作为对应这个关系的谓词名。如果$(a_1, a_2,...,a_n)$是满足R的元组，那么元组$R(a_1,a_2, ...,a_n)$的值为True，否则元组的值为False。<font color=red><b>需要注意，谓词定义的关系都是集合关系，之后再讨论将Datalog扩展到包</b></font>



**算术原子(arithmetic atom)**

算术原子是对两个算术表达式作比较，例如 x< y或$x+1 \gt y+4 \times z$



**Datalog规则和查询**

和关系代数类似的操作在Datalog中称作规则(rule)，它包括：

1. 一个称为头部(head)的关系原子
2. 符号$\longleftarrow$, 经常读作if
3. 主体(body)部分，由一个或多个称为子目标(subgoal)的原子组成。原子可以是关系原子或算术元组。子目标之间由AND连接，任何目标之前都可以随意添加逻辑算子NOT。

Datalog中规则适用变量是有限制的，该限制是要使得一条规则的结果是一个有限的关系，从而包含算术子目标或否定(negated)子目标的规则更有直观意义，这个限制条件称作**安全(safety)条件**，含义是:

- 每个在规则中任意位置出现的变量都必须出现在主体的某些非否定的关系子目标中。

尤其是，任何在规则头部，否定关系子目标或任意算术子目标中出现的变量，也必须出现在主体的非否定的关系子目标中。



Datalog规则的意义：假设规则变量涉及所有可能的值，只要这些变量的值使得所有子目标为真，那么对应于这些变量的规则头部的值就清楚了，并可把结果元组加入到头部谓词关系中。另外一种定义规则的意义：不去考虑所有可能的变量赋值，而是考虑对应于每个非否定关系子目标的关系的元组集合。如果每个非否定关系子目标的某些元组的赋值是一致(consistent)的，也就是说，对一个变量的每次出现都赋同一个值，则考虑对规则所有变量的结果赋值。因为规则是安全的，所以每个变量都被赋了一个值。

## 扩展谓词和内涵谓词

- 扩展谓词(Extensional Predicate):这种谓词的关系存放在数据库中，其对应的关系是EDB(Extensional Database, 扩展数据库的缩写)。可以出现在规则主体中，但不能出现在规则头部。

- 内涵谓词(Intension Predicate):这种谓词关系由一个或多个Datalog规则计算出来，它对应的关系是IDB(Intension Database,内涵数据库)。它可以出现在规则的头部和主体中，也可以同时出现在两个位置。

这两种谓词间的区别等同于关系代数表达式的操作数与关系代数表达式计算出的关系间的区别。前者，关系代数表达式操作数是”可扩展的“，也就是说通过它的扩展定义关系，也即“关系的当前实例”的另一命名；后者关系代数表达式计算出的关系可以是最终结果也可以是对应某些子表达式的中间结果，这些关系是”内涵的“(即由程序员的意图决定的)。



## Datalog规则应用于包

Datalog本质上是集合逻辑，然而只要没有否定的关系子目标，关系是集合时计算Datalog规则的方法对于关系是包时同样适用。由于现在是处理包，就不消除头部的重复元组，而且当考虑子目标的所有元组的组合时，一个子目标关系中重复出现n次的一个元组在被作为该子目标元组与其他子目标元组合并时也被处理了n次。



# 关系代数与Datalog
将算子和Datalog组合来模拟复杂代数表达式，事实上任何一个安全Datalog规则都可以用关系代数表达。当允许多个规则交互作用时，Datalog查询比关系代数功能更强，它能表达代数中不能表达的递归功能。

## 布尔操作

关系代数不二操作--并、交、差可以简单地用Datalog表达。假设R和S是具有相同多的属性n的关系，使用Answer作为头谓词名字。

$R \cup S$使用两条规则和n个不同变量:
$$
Answer(a_1, a_2,..., a_n) \longleftarrow R(a_1, a_2,..., a_n) \\
Answer(a_1, a_2,..., a_n) \longleftarrow S(a_1, a_2,..., a_n)
$$
$R \cap S$使用主体规则:
$$
Answer(a_1, a_2,..., a_n) \longleftarrow R(a_1, a_2,..., a_n) AND S(a_1, a_2,..., a_n)
$$
$R - S$使用主体规则
$$
Answer(a_1, a_2,..., a_n) \longleftarrow R(a_1, a_2,..., a_n) \ AND \ NOT \ S(a_1, a_2,..., a_n)
$$


## 投影

计算关系R的投影，使用只有一条谓词R的单子目标规则。该子目标参数是不同变量，每个代表关系的一个属性。规则头部有一个原子，其参数按照期望顺序对应于投影列表，例如将关系$Movies(title, year, length, genre, studioName, producerC\#)$投影到title,year, length三个属性上，规则是$P(t, y, l) \longleftarrow Movies(t, y, l, g, a, p)$

## 选择

当选择条件是对一个或多个算术比较来作And操作时，创建规则包含：

1. 一个对应于要进行选择的关系的子目标。该原子对每个分量有不同变量，它们分别对应于关系的每个属性。
2. 选择条件中的每个比较都有一个与该比较对应的算术子目标。而且，一旦选择条件中使用了一个属性名，就根据关系子目标建立的对应关系在算术子目标中使用对应的变量

例如，关系代数：$\sigma_{length \ge 100 \ AND \ studioName='Fox'}(Movies)$用Datalog规则可以重写为
$$
S(t, y, l, g, s, p) \longleftarrow Movies(t, y, l, g, s, p) \ AND \ l \ge 100 \ AND \ s='Fox' \\
$$


两个OR操作的选择近似于分别按照每个条件进行选择，然后得到结果的并。n个条件的OR可以用n条规则表示，每个规则定义相同的头部谓词，第i跳规则按照n个条件中的第i个进行选择。

例如关系代数：$\sigma_{length \ge 100 \ OR \ studioName='Fox'}(Movies) \\$用Datalog规则可以重写为
$$
\begin{array}{l}
S(t, y, l, g, s, p) \longleftarrow Movies(t, y, l, g, s, p) \ AND \ l \ge 100 \\
S(t, y, l, g, s, p) \longleftarrow Movies(t, y, l, g, s, p) \ AND \ s='Fox'
\end{array}
$$


对于复杂的选择条件，可以由逻辑算子AND、OR和NOT按照任意顺序组成。有一种技术可以将这样的逻辑表达式重新整理成“析取范式”，即表达式是合取的析取(disjunt)是OR。合取(conjunt)是AND，而文字(literals)是一种比较或否定的比较。

## 积

$R \times S$可以用一条Datalog规则表示。头部IDB谓词拥有所有在两个子目标中出现的参数，R子目标中的变量列在S子目标中变量的前面。
$$
P(a, b, c, x, y, z) \longleftarrow R(a, b, c)\  AND \ S(x, y, z)
$$


## 连接

可以使用一条近似于积的Datalog规则来表示两个关系的自然连接，如果想要得到$R \Join S$,则R和S的同名属性要使用形同的变量，否则使用不同的变量。例如R(A,B)和S(B,C, D)的自然连接可以采用$J(a, b, c, d) \longleftarrow R(a, b) AND S(b, c, d)$



$\theta$连接可以采用一个积后接选择操作符来转换成Datalog，如果选择条件是合取(即AND)，可以简单地从积规则开始，再添加对应于每个比较的算术目标。例如
$$
\begin{array}{l}
关系U(A, B, C)和V(B, C, D)进行\theta连接 U \Join_{A < D \ AND \ U.b \neq V.B} V \\
J(a, ub, uc, vb, vc, d) \longleftarrow U(a, ub, uc) \ AND \ V(vb, vc, d) \ AND \ a < d \ AND \ ub \neq vb
\end{array}
$$


$\theta$连接的连接条件不是合取，可以将它转化为析取范式，然后为每个合取建立规则
$$
\begin{array}{l}
关系U(A, B, C)和V(B, C, D)进行\theta连接 U \Join_{A < D \ OR \ U.b \neq V.B} V \\
\\
采用变量命名可以得到两条规则: \\
1.\  J(a, ub, uc, vb, vc, d) \longleftarrow U(a, ub, uc) \ AND \ V(vb, vc, d) \ AND \ a < d \\
2.\  J(a, ub, uc, vb, vc, d) \longleftarrow U(a, ub, uc) \ AND \ V(vb, vc, d) \ AND \ ub \neq vb
\end{array}
$$

# 参考资料

1. 数据库系统基础教程(第3版)