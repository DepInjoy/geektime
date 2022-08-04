通过统计信息，代价估算系统就可以了解一个表有多少行数据、用了多少个数据页面、某个值出现的频率等，然后根据这些信息计算出一个约束条件能过滤掉多少数据，该约束条件过滤出的数据占总数据量的比例称为选择率。
$$
选择率 = \frac{约束条件过滤后的元组数}{约束条件过滤前的总元组数}
$$
# 统计信息和选择率
## 统计信息
<center>
    <div><b>单列统计信息</b></div>
</center>

| 统计信息                          | 类型定义                              | 含义                                                         |
| --------------------------------- | ------------------------------------- | ------------------------------------------------------------ |
| **空值率(NULL值率)**              |                                       | 列中NULL值所占的比例                                         |
| **平均宽度**                      |                                       | 列数据的平均宽度                                             |
| **高频值/常见值(MCV/MCF)**        | STATISTIC_KIND_MCV                    | 在一个列里出现最频繁的值，按照出现的频率进行排序，并且生成一一对应的频率数组，这样可以知道一个列中有哪些高频值以及这些高颇值的频率 |
| **基数计数(Distinct)**            |                                       | 列中不重复值的个数或比例，在PG中取值有三种情况:<br/>1.代表未知或者未计算的情况。<br/>2.>0，代表消除重复值之后的个数，不常使用这种情况<br/>3.<0，其绝对值是去重之后的个数占总 的比例， 通常使用这种类型。 |
| **直方图(Histogram)**             | STATISTIC_KIND_HISTOGRAM              | PostgreSQL数据库使用等频直方图来描述一个列中的数据的分布，高颇值不会出现在直方图中，这样保证数据分布相对平坦 |
| 相关系数                          | STATISTIC_KIND_CORRELATION            | 相关系数记录的是当前列未排序的数据分布和排序后的数据分布的相关性，这个值通常在索引扫描时用来估计代价，假设一个列未排序和排序之后的相关性是0，也就是完全不相关，那么索引扫描的代价就会高一些 |
| 类型高频值                        | STATISTIC_KIND_MCELEM                 | 用于数组类型或者一些其他类型，PostgreSQL数据库提供了`ts_typanalyze`系统函数来负责生成这种类型的统计信息 |
| 数组类型高频值                    | STATISTIC_KIND_DECHIST                | 用于给数组类型生成直方图,PG提供了`array_typanalyze`系统函数来负责生成这种类型的统<br/>计信息 |
| Range类型基于长度的直方图统计信息 | STATISTIC_KIND_RANGE_LENGTH_HISTOGRAM | 用户可以自定义Range类型，PG提供了`range_typanalyze`系统函数负责生成该类型的统计信息 |
| Range类型基于边界的直方图         | STATISTIC_KIND_BOUNDS_HISTOGRAM       | Range类型生基于边界的直方图,这种类型直方图通过`range_typanalyze`系统函数来进行统计 |

<center>
    <div><b>多列统计信息</b></div>
</center>

| 统计信息           | 类型                   | 说明                                                         |
| ------------------ | ---------------------- | ------------------------------------------------------------ |
| 多列消重后的数据量 | `STATS_EXT_NDISTINCT`  | 和单列统计信息中的`staditinct`是类似的,`staditinct`记录的是单列中去掉NULL 值和消重之后的数据量或者比例，`STATS_EXT_NDISTINCT`类型的统计信息则记录的是基于多列的消重之后的数据量 |
| 列间依赖度         | STATS_EXT_DEPENDENCIES | 计算各个列之间的函数依赖度，通过函数依赖度计算各个列之间的依赖关系，从而得到准确的统计信息 |

在`optimizer\cost.h`定义了不同的代价
```c
#define DEFAULT_SEQ_PAGE_COST  1.0  		// 顺序读单页代价
#define DEFAULT_RANDOM_PAGE_COST  4.0		// 随机读单页代价
#define DEFAULT_CPU_TUPLE_COST	0.01		
#define DEFAULT_CPU_INDEX_TUPLE_COST 0.005
#define DEFAULT_CPU_OPERATOR_COST  0.0025
#define DEFAULT_PARALLEL_TUPLE_COST 0.1
#define DEFAULT_PARALLEL_SETUP_COST  1000.0
```
单列统计类型
```c
#define STATISTIC_KIND_MCV 1 
#define STATISTIC_KIND_HISTOGRAM 2 
#define STATISTIC_KIND_CORRELATION 3 
#define STATISTIC_KIND_MCELEM 4 
#define STATISTIC_KIND_DECHIST 5 
#define STATISTIC_KIND_RANGE_LENGTH_HISTOGRAM 6 
#define STATISTIC_KIND_BOUNDS_HISTOGRAM 7
```
多列统计类型
```c
#define STATS EXT NDISTINCT     'd' 
#define STATS EXT DEPENDENCIES  'r'
```
## 选择率
### 