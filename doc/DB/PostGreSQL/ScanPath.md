#  扫描路径

##  单表查询代价估计(Cost)

PostgreSQL的查询优化是基于代价的。代价是一个无量纲的值，它不是一种绝对的性能指标，但可以作为比较各种操作代价时的相对性能指标。
`costsize.c` 中的函数用于估算各种操作的代价。所有被执行器执行的操作都有着相应的代价函数。

- `cost_seqscan()`用于估算顺序扫描代价
- `cost_index()`用于估算索引扫描的代价。

在PostgreSQL中有三种代价，分别是启动代价、运行代价和总代价。总代价是启动代价和运行代价的和，只有启动代价和运行代价是单独估计的。

- 启动代价：在读取到第一条元组前花费的代价，比如索引扫描节点的启动代价就是读取目标表的索引页，获取到第一个元组的代价。
- 运行代价：获取全部元组的代价。

### 1. 顺序扫描

顺序扫描的代价是通过函数`cost_seqscan()`估计的。
$$
\begin{array}{l}
run\_cost \\
= cpu\_run\_cost+disk\_run\_cost \\
= (cpu\_tuple\_cost+cpu\_operator\_cost)×N_{tuple} + seq\_page\_cost × N_{page} \\
\\
seq\_page\_cost、cpu\_tuple\_cost和cpu\_operator\_cost是在postgresql.conf中配置的参数\\
默认值分别为1.0、0.01和0.0025 \\
N_{tuple}和N_{page}分别是表中的元组总数与页面总数

\end{array}
$$


### 2. 索引扫描

#### 启动代价

$$
\begin{array}{l}
start\_up\_cost={ceil(log_2(N_{index, tuple}))+(H_{index}+1)×50}×cpu_operator_cost \\
其中,H_{index}是索引树的高度
\end{array}
$$

#### 运行代价

运行代价是表和索引的CPU代价和I/O代价之和。
$$
\begin{array}{l}
run\_cost=(index\_cpu\_cost+table\_cpu\_cost)+(index\_io\_cost+table\_io\_cost) \\
 \\
其中:\\
index\_cpu\_cost=Selectivity×N_{index,tuple}×(cpu\_index\_tuple\_cost+qual\_op\_cost) \\
table\_cpu\_cost=Selectivity×N_{tuple}×cpu\_tuple\_cost \\
index\_io\_cost=ceil(Selectivity×N_{index,page})×random\_page\_cost \\
\\
cpu\_index\_tuple\_cost和random\_page\_cost在postgresql.conf中配置（默认分别为0.005和4.0) \\
qual\_op\_cost粗略来说就是索引求值的代价，默认值是0.0025 \\
Selectivity选择率是一个0到1之间的浮点数，代表查询指定的WHERE子句在索引中搜索范围的比例 \\
(Selectivity×N_{tuple})需要读取表元组数量 \\
(Selectivity×N_{index,page}）需要读取索引元组数量 \\
\end{array}
$$

##### 选择率

选择率

> 查询谓词的选择率是通过直方图界值与高频值估计的，这些信息都存储在系统目录`pg_statistics`中，并可通过`pg_stats`视图查询,表中每一列的高频值都在`pg_stats`视图的`most_common_vals`和`most_common_freqs`中成对存储。
>
> - 高频值：该列上最常出现的取值列表
> - 高频值频率：高频值相应出现频率的列
>
> 如果高频值不可用，就会使用目标列上的直方图界值来估计代价。直方图值是一系列值，这些值将列上的取值划分为数量大致相同的若干个组。默认情况下，直方图界值会将列上的取值划分入100个桶[1]。

$$
\begin{array}{l}
table\_io\_cost = max\_io\_cost+indexCorerelation^2×（min\_io\_cost-max\_io\_cost）\\
 \\
其中：\\
max\_io\_cost = N_{page}×random\_page\_cost,是最差情况下的 I/O代价，即随机扫描所有数据页的代价 \\
min\_io\_cost是最优情况下的I/O代价，即顺序扫描选定的数据页 \\
min\_io\_cost = 1×random\_page\_cost +(ceil(Selectivity× N_{page})-1)×seq\_page\_cost \\
\end{array}
$$

##### 索引相关性

> 索引相关性是列值在物理上的顺序和逻辑上的顺序的统计相关性。索引相关性的取值范围从-1到+1。
>
> 索引相关性是一种统计上的相关性。在索引扫描代价估计中，索引相关性体现了索引顺序和物理元组顺序扭曲程度给随机访问性能造成的影响大小[1]。

#### 	整体代价



> `seq_page_cost`和`random_page_cost`的默认值分别为1.0和4.0。这意味着PostgreSQL假设随机扫描的进度是顺序扫描的1/4；显然，PostgreSQL的默认值是基于HDD（普通硬盘）设置的。近年来SSD得到了广泛的应用，random_page_cost的默认值就显得太大了。使用SSD时如果仍然采用`random_page_cost`的默认值，则计划器有可能会选择低效的计划。因此当使用SSD时最好将`random_page_cost`的值设为1.0。
>
> https://amplitude.engineering/how-a-single-postgresql-config-change-improved-slow-query-performance-by-50x-85593b8991b0这篇文章报告了使用random_page_cost默认值导致的问题[1]。



### 3.排序

# 参考资料

1. PostgreSQL指南:内幕探索



