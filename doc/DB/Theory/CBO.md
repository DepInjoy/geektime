# 查询代价估算

## 代价模型

查询代价估算基于CPU代价和IO代价，所以代价模型可以用以下计算公式表示：
$$
\begin{array}{l}
总代价 = IO 代价 + CPU 代价 \\
COST = P*a\_page\_cpu\_time + W * T \\
\end{array}
$$
其中：

- P为计划运行时访问的页面数
- $a\_page\_cpu\_time$是每个页面读取的时间花费，$P * a\_page\_cpu\_time$反映了IO代价。
- T为访问的元组数，反映了CPU花费（存储层是以页面为单位，数据以页面的形式被读入内存，每个页面上可能有多条元组，访问元组需要解析元组结构，才能把元组上的字段读出，这消耗的是CPU）。如果是索引扫描，则还会包括索引读取的花费。
- W为权重因子，表明IO到CPU的相关性，又称选择率(selectivity)。选择率用于表示在关系R中，满足条件`A<op>a`的元组数与R的所有元组数N的比值。

## 选择率

选择率计算的常用方法有

- 无参数方法（Non-Parametric Method）。使用ad hoc数据结构或直方图维护属性值的分布，最常用的是直方图方法
- 参数法（Parametric Method）。使用具有一些自由统计参数（参数是预先估计出来的）的数学分布函数逼近真实分布。
- 曲线拟合法（Curve Fitting）。为克服参数法的不灵活性，用一般多项式和标准最小方差来逼近属性值的分布。
- 抽样法（sampling）。从数据库中抽取部分样本元组，针对这些样本进行查询，然后收集统计数据，只有足够的样本被测试之后，才能达到预期的精度。
- 综合法。将以上几种方法结合起来，如抽样法和直方图法结合。

|统计信息|含义|实现算法|
|--|--|--|
|空值率(NULL frac)|NULL值在整列中的占比|遍历|
|平均宽度(avg width)|数据的平均宽度|遍历|
|基数计数(distinct)|列中不重复值的个数|HLL|
|常见值(MCV/MCF)|至多前100个常见值及其频率|遍历|
|直方图(histogram)|除MCV后的等间距分割点(至多100个)|排序|
# 单表扫描算法

单表扫描需要从表上获取元组，直接关联到物理IO的读取，所以不同的单表扫描方式，有不同的代价。常用的单表扫描方式：

- 全表扫描表数据。为获取表的全部元组，读取表对应的全部数据页，通常采取顺序读取的算法。
- 局部扫描表数据。为获取表的部分元组，读取指定位置对应的数据页。
  

常用的单表扫描算法：

- 顺序扫描（SeqScan）。从物理存储上按照存储顺序直接读取表的数据；当无索引可用，或访问表中的大部分数据，或表的数据量很小时，使用顺序扫描效果较好。
- 索引扫描（IndexScan）。根据索引键读索引，找出物理元组的位置；根据从索引中找到的位置，从存储中读取数据页面；索引扫描可以将元组按排序的顺序返回；索引扫描时若选择率较低，则读数据花费的IO会显著减少；换句话说，如果选择率很高，不适宜使用索引扫描。
- 只读索引扫描（IndexOnlyScan）。根据索引键读索引，索引中的数据能够满足条件判断，不需要读取数据页面；比索引扫描少了读取数据的IO花费。
- 行扫描（RowIdScan）。用于直接定位表中的某一行。对于元组，通常为元组增加特殊的列，可以通过特殊的列计算出元组的物理位置，然后直接读取元组对应的页面，获取元组；在PostgreSQL中称为Tid扫描，此种方式是在元组头上增加名为CTID的列，用这列的值可以直接计算本条元组的物理存储位置。
- 并行表扫描（ParallelTableScan）。对同一个表，并行地、通过顺序的方式获取表数据，结果得到一个完整表数据。
- 并行索引扫描（ParallelIndexScan）。对同一个表，并行地、通过索引的方式获取表的数据，将结果合并在一起。
- 组合多个索引扫描。有的系统称为MultipleIndexScan。对同一个元组的组合条件（AND或者OR谓词组合的多个索引列上的多条件查询）进行多次索引扫描，然后在内存里组织一个位图，用位图描述索引扫描结果中符合索引条件的元组位置。组合多个索引（包括同一索引的多次使用）来处理单个索引扫描不能实现的情况。本质上不是单表的扫描方式，是构建在单表的多个索引扫描基础上的。

对于局部扫描，根据数据量的情况和元组获取条件，可能采用顺序读取或随机读取存储系统的方式。此时，选择率有一定影响。如果选择率的值很大，意味着采取顺序扫描方式可能比局部扫描的随机读的方式效率更高，由于顺序IO会减少磁盘头移动的等待时间，如果数据库文件在磁盘上没有碎片，对性能的改善将更明显。对于大表，顺序扫描会一次读取多个页，这将进一步降低顺序表扫描的开销。
对于局部扫描，通常采用索引，实现少量数据的读取优化，是一种随机读取数据的方式。虽然顺序表扫描可能会比读取许多行的索引扫描花费的时间少，但如果顺序扫描被执行多次，且不能有效地利用缓存，则总体花费巨大。索引扫描访问的页面可能较少，而且这些页很可能会保存在数据缓冲区，访问的速度会更快。所以，对于重复的表访问（如嵌套循环连接的右表），采用索引扫描比较好。究竟采取哪种扫描方式，查询优化器在采用代价估算比较代价的大小后才决定。

## 单表扫描代价计算

$$
\begin{array}{l}
顺序扫描 \\
cost = N\_page * a\_tuple\_IO\_time + N\_tuple * a\_tuple\_CPU\_time \\
\\
索引扫描 \\
cost = C\_index + N\_page\_index * a\_tuple\_IO\_time
\end{array}
$$

其中：

- $a\_page\_IO\_time$，一个页面的IO花费。
- $N\_page$，数据页面数。
- $N\_page\_index$，索引页面数。
- $a\_tuple\_CPU\_time$，一个元组从页面中解析的CPU花费。
- $N\_tuple$，元组数。
- $C\_index$，索引的IO花费，$C\_index=N\_page\_index * a\_page\_IO\_time$
- $N\_tuple\_index$，索引作用下的可用元组数,$N\_tuple\_index=N\_tuple * 索引选择率$

# 两表连接算法
连接操作CPU Cost
|算法|CPU代价估算公式|
|:--|:--|
|嵌套循环算法 | 基本嵌套循环算法：$C\_outer + C\_inner$<br/>内表使用索引改进嵌套循环算法：$C\_outer + C\_inner\_index$|
|归并连接|基本归并连接:$C\_outer + C\_inner + C\_outersort + C\_innersort$<br/>内表使用索引,只影响排序,$C\_outersort$ + $C\_innersort$可能会变化|
|Hash连接|$C\_createHash + (N\_outer * N\_inner *选择率) * a\_tuple\_cpu\_time$|

其中：
- $a\_tuple\_cpu\_time$，获取一个元组消耗的CPU时间。
- $N\_outer$, 扫描获取的外表元组数
- $N\_inner$, 扫描获取的内表元组数，$N\_inner = N\_inner\_all * 选择率$，其中$N\_inner\_all$表示内表的所有元组数。
- $C\_outer$, 扫描外表的代价，$C\_outer = N\_outer * a\_tuple\_cpu\_time$。
- $C\_inner$，扫描内表的代价，$C\_inner=N\_inner * a\_tuple\_cpu\_time$。
- $C\_inner\_index$，使用索引扫描内表的代价，通常$C\_inner\_index$会小于$C\_inner$。
- $C\_outersort$，外表排序的代价。
- $C\_innersort$，内表排序的代价。
- $C\_CreateHash$，创建Hash的代价。

# 多表连接
多表间的连接顺序表示了查询计划树的基本形态。一棵树就是一种查询路径，SQL的语义可以由多棵这样的树表达，从中选择花费最少的树，就是最优查询计划形成的过程。一棵树包括左深连接树、右深连接树、紧密树(1990年, Schneder等在研究查询树模型时提出了左深树Left Deep Trees、右深树Right Deep Trees和紧密树bushy Trees)3种形态
<center>
<img src="./img/JoinOrderTreeForm.png">
<div><b>计划树的三种形态</b></div>
</center>


不同的连接顺序，会生成不同大小的中间关系，这意味着CPU和IO消耗不同, PostgreSQL中会尝试多种连接方式存放到path上，以找出花费最小的路径。

另外，即使是同一种树的生成方式，也有细节需要考虑。例如上图a中，`{A，B}`和`{B，A}`两种连接方式花费可能不同。比如最终连接结果是`{A，B，C}`，但是需要验证是`{A，B，C}`、`{A，C，B}`、`{B，C，A}`、`{B，A，C}`、`{C，A，B}`、`{C，B，A}`中哪一个连接方式得到的结果，无论是哪种结果，都需要计算这6种连接方式中每一种的花费，找出最优的一种作为下次和其他表连接的依据。

针对以上树的形成、形成的树的花费代价最少的，提出了诸多算法。树的形成过程，主要有以下两种策略：
- 至顶向下。从SQL表达式树的树根开始，向下进行，估计每个结点可能的执行方法，计算每种组合的代价，从中挑选最优的。
- 自底向上。从SQL表达式树的树叶开始，向上进行，计算每个子表达式的所有实现方法的代价，从中挑选最优的，再和上层（靠近树根）的进行连接，周而复始直至树根。

常见的多表连接实现算法有：
| 算法           | 特点与使用范围                                               | 缺点                             | 策略     | 实际使用    |
| -------------- | ------------------------------------------------------------ | -------------------------------- | -------- | ----------- |
| 启发式算法     | 适用于任何范围，与其他算法结合，可以有效提高整体效率         | 不知道得到的解是不是最优         |          |             |
| 贪婪算法       | 非穷举类型算法，适合解决较多关系的搜索                       | 得到局部最优解                   | 自顶向下 | MySQL和TiDB |
| 爬山算法       | 适合关系中包含较多关系的搜索，基于贪婪算法                   | 随机性强，得到局部最优解         |          |             |
| 遗传算法       | 非穷举类型算法，适合解决较多关系的搜索                       | 得到局部最优解                   |          |             |
| 动态规划算法   | 穷举类型的算法，适合查询中包含较少关系的搜索，可得到全局最优解 | 搜索空间随关系个数增长呈指数增长 | 自底向上 | PostGreSQL  |
| System R优化   | 基于自底向上的动态规划算法，为上层提供更多可能的备选路径，可得到全局最优解。 | 搜索空间可能比动态规划算法更大   | 自底向上 |             |
| 分支界定枚举法 |                                                              |                                  |          |             |
| 随机算法       |                                                              |                                  |          |             |
| 模拟退火算法   |                                                              |                                  |          |             |

## 动态规划
20世纪40年代,Richard Bellman最早使用了动态规划这一概念, 用以表述通过遍历寻找最优决策解问题。规划的含义是指生成活动的优化策略，规划意味着找到一个可行的活动计划。动态规划，是指决策依赖于当前状态，又随即引起状态的转移，一个决策序列就是在变化的状态中产生出来的，这就是“动态”的含义。

“动态规划”将待求解的问题分解为若干个子问题，按顺序求解子问题(子阶段)，前一子问题的解为后一子问题的求解提供了有用的信息。在求解任一子问题时，列出各种可能的局部解，通过决策保留那些有可能达到最优的局部解，丢弃其他局部解。依次解决各子问题，最后一个子问题就是初始问题的解。

动态规划算法包括的主要概念：
- **阶段**: 把求解问题的过程分成若干个相互联系的阶段，以便于求解。在多数情况下，阶段变量是离散的。
- **状态**。表示每个阶段开始面临的自然状况或客观条件，它不以人们的主观意志为转移，也称为不可控因素。
- **无后效性**。状态应该具有的性质，如果给定某一阶段的状态，则在这一阶段以后过程的发展不受该阶段以前各段状态的影响。
- **决策**。一个阶段的状态确定后，从该状态演变到下一阶段某个状态的选择（行动）称为决策。
- **策略**。由每个阶段的决策组成的序列称为策略。对于每一个实际的多阶段决策过程，可供选取的策略有一定的范围限制，这个范围称为允许策略集合。允许策略集合中达到最优效果的策略称为最优策略。
- **最优化原理**。如果问题的最优解所包含的子问题的解也是最优的，就称该问题具有最优子结构，即满足最优化原理。最优化原理实际上是要求问题的最优策略的子策略也是最优的

**动态规划算法是从底向上进行的，即从叶子(单个表)开始算作一层，然后由底层开始对每层的关系做两两连接（如果满足内连接则两两连接，不满足内连接则不可对全部表进行两两连接操作），构造出上层，逐次递推到根部。** 具体步骤:
1. 初始状态。构造第一层关系，即叶子结点，每个叶子对应一个单表，为每一个待连接的关系计算最优路径(单表的最优路径就是单表的最佳访问方式，通过评估不同的单表的数据扫描方式花费，找出代价最小的作为每个单表的局部最优路径)。
2. 归纳。假设当层数从第1到n-1已经生成，求解第n层的关系，有两种方式
   - 左深树连接方式：将第n-1层的关系与第一层中的每个关系连接，生成新的关系(对新关系的大小进行估算)，放于第n层，且每一个新关系，均求解其最优路径。
   - 紧密树连接方式：将第k层的每个关系与第level-k（level=总的层数=要连接的基表个数）层中的每个关系连接，生成新的关系（新的关系就存储着形成这个关系的多种局部路径，从中选出最优的一个局部路径）放于第n层，且每一个新关系，均求解其最优路径。

以上虽然分为两步，但实际上步骤2多次执行，每一次执行后生成的结果被下一次使用，即每层路径的生成都是基于上层生成的最优路径的，这满足最优化原理的要求。

例如,下面的查询语句
```sql
SELECT * FROM A, B, C, D
    WHERE A.col=B.col AND A.col=C.col AND A.col=D.col;
```

| 层数 | 说明                                                         | 可能的得到的连接的中间结果                                   | 左深树                  | 紧密树                       |
| ---- | ------------------------------------------------------------ | ------------------------------------------------------------ | ----------------------- | ---------------------------- |
| 4    | 第四层可以通过第一、三层或第二、二层，进行每层间的两个表两两连接得到。 | {A,B,C,D}                                                    | 用第一层与第三层关系连接 | 第二层每个关系与其他关系连接 |
| 3    | 第三层可以通过第一层和第二层两两连接得到。第三层得到的最终中间连接结果不多，但同一个中间结果可能有多重连接组合得到，例如{A, B, C}可以由{A,B}, {A,C}或{A,B}, {B,C}连接得到，也可以由{A,C},{C,B}或{A,C}, {B,C}等得到，经过Cost估算选取Cost最小的。 | {A,B,C}, {A,B,D}, {A,C,D}, {B,C,D}                           |      第一层和第二层两两连接                   | 不满足                       |
| 2    | 第二层可以通过第一层中的表两两连接得到{A,B}, {B, A}可能的连接代价是不同的，因此算作两个候选项 | {A,B}, {B,A}, {A,C}, {C,A}, {A, D}, {D, A}, {B,C}, {C, B}, {B, D}, {D, B},{C, D}, {D, C} | 第一层中的表两两连接 | 不满足                       |
| 1    | 树叶，初始层，每个单表计算单表扫描代价                       | {A}, {B}, {C}, {D}                                           || 不满足                       |

## 贪婪算法
贪婪(Greedy)算法，又称贪心算法。在对问题求解时，贪婪算法总是做出在当前看来是最好的选择，而这种选择是局部最优。局部最优不一定是整体最优，它不从整体最优上加以考虑，省去了为找最优解要穷尽所有可能而必须耗费的大量时间（这点正是动态规划算法所做的事情），得到的是局部最优解。

贪婪算法为了解决问题需要寻找一个构成解的候选对象集合。其主要实现步骤如下：
1. 初始，算法选出的候选对象的集合为空
2. 根据选择函数，从剩余候选对象中选出最有希望构成解的对象
3. 如果集合中加上该对象后不可行，那么该对象就被丢弃并不再考虑
4. 如果集合中加上该对象后可行，就加到集合里
5. 扩充集合，检查该集合是否构成解
6. 如果贪婪算法正确工作，那么找到的第一个解通常是最优的，可以终止算法
7. 继续执行2(每做一次贪婪选择就将所求问题简化为一个规模更小的子问题，最终可得到问题的一个可能的整体最优解)

## SystemR算法
主要的思想是把子树的查询计划的最优查询计划和次优的查询计划保留，用于上层的查询计划生成，以便使得查询计划总体上最优。

## 遗传算法
遗传算法(Genetic Algorithm, GA)是美国学者Holland于1975年首先提出来的, 是一种启发式的优化算法，是基于自然群体遗传演化机制的高效探索算法。

遗传算法抛弃了传统的搜索方式，模拟自然界生物进化过程，采用人工进化的方式对目标空间进行随机化搜索。它将问题域中的可能解看作是群体的一个个体（染色体），并将每一个个体编码成符号串形式，模拟达尔文的遗传选择和自然淘汰的生物进化过程，对群体反复进行基于遗传学的操作（选择、交叉、变异），根据预定的目标适应度函数对每个个体进行评价，依据“适者生存，优胜劣汰”的进化规则，不断得到更优的群体，同时以全局并行搜索方式来搜索优化群体中的最优个体，求得满足要求的最优解。

遗传算法可以有效地利用已经有的信息处理来搜索那些有希望改善解质量的串，类似于自然进化，遗传算法通过作用于“染色体”上的“基因”，寻找好的“染色体”来求解问题（对算法所产生的每个“染色体”进行评价，并基于适应度值来改造“染色体”，使适用性好的“染色体”比适应性差的“染色体”有更多的“繁殖机会”）

遗传算法的主要概念:
- 群体(population)。一定数量的个体组成了群体，表示GA的遗传搜索空间。
- 个体(individual)。多个个体组成群体，在多表连接中是每个基本关系或中间生成的临时关系。
- 染色体(chromosome)。个体的特征代表，即个体的标志，由若干基因组成，是GA操作的基本对象，所以操作个体实则是操作染色体（个体几乎可以简单理解为“等同染色体”）。染色体用字符串表示。
- 基因(gene)。基因是染色体的片段，多段基因组成染色体，基因变异导致基因不断被优化。
- 适应度(fitness)。表示个体对环境的适应程度，通常由某一适应度函数表示。对应执行策略的执行代价。
- 选择(selection)。GA的基本操作之一，即根据个体的适应度，在群体中按照一定的概率选择可以作为父本的个体，选择依据是适应度大的个体被选中的概率高。选择操作体现了“适者生存，优胜劣汰”的进化规则。
- 交叉(crossover)。GA的基本操作之一，即将父本个体按照一定的概率随机地交换基因形成新的个体。
- 变异(mutate)。GA的基本操作之一，即按一定概率随机改变某个个体的基因值。

遗传算法的关键问题
- 串的编码方式。本质是编码问题。一般把问题的各种参数用二进制形式进行编码，构成子串；然后把子串拼接构成“染色体”串。串长度及编码形式对算法收敛影响极大。
- 适应度函数的确定。适应度函数(fitness function)又称对象函数(object function)或问题的“环境”，是问题求解品质的测量函数。一般可以把问题的模型函数作为对象函数，但有时需要另行构造。
- 遗传算法自身参数设定。遗传算法自身参数有3个，即群体大小$n$、交叉概率$P_c$和变异概率$P_m$，具体如下：
  - 群体大小n太小时难以求出最优解，太大则增长收敛时间，一般$n=30~160$。
  - 交叉概率Pc太小时难以向前搜索，太大则容易破坏高适应值的结构，一般取$P_c=0.25~0.75$
  - 变异概率Pm太小时难以产生新的基因结构，太大使遗传算法成了单纯的随机搜索，一般取$P_m=0.01~0.2$。

遗传算法主要步骤如下
1. 随机初始化种群
2. 评估初始的种群，即为种群计算每个个体的适应值且对所有个体排序
3. 如果没有达到预定演化数(可以是一个确定的、与连接的表的个数无关的值，这样保证搜索空间一定不会因连接的表的个数增多导致搜索空间指数级增大)，则继续下一步，否则结束算法
4. 选择父体，随机挑选父体dad和母体mum
5. 杂交，父体和母体杂交得到新个体child
6. 变异，在某些个别条件下对新个体变异(不是大概率变异，不是每次都需要变异)
7. 计算新个体的适应值，并把适应值排名插入到种群，种群中排名最后的则被淘汰
8. 继续步骤3。

# 参考资料

- 数据库查询优化器的艺术:原理分析和SQL性能优化