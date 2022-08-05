通过统计信息，代价估算系统就可以了解一个表有多少行数据、用了多少个数据页面、某个值出现的频率等，然后根据这些信息计算出一个约束条件能过滤掉多少数据，该约束条件过滤出的数据占总数据量的比例称为选择率。
$$
选择率 = \frac{约束条件过滤后的元组数}{约束条件过滤前的总元组数}
$$
# 统计信息和选择率
## 统计信息
<center>
    <div><b>单列统计信息</b></div>
</center>

| 统计信息                          | 含义                                                         |
| --------------------------------- | ------------------------------------------------------------ |
| **空值率(NULL值率)**              | 列中NULL值所占的比例                                         |
| **平均宽度**                      | 列数据的平均宽度                                             |
| **高频值/常见值(MCV/MCF)**        | 在一个列里出现最频繁的值，按照出现的频率进行排序，并且生成一一对应的频率数组，这样可以知道一个列中有哪些高频值以及这些高颇值的频率 |
| **基数计数(Distinct)**            | 列中不重复值的个数或比例，在PG中取值有三种情况:<br/>1.代表未知或者未计算的情况。<br/>2.>0，代表消除重复值之后的个数，不常使用这种情况<br/>3.<0，其绝对值是去重之后的个数占总 的比例， 通常使用这种类型。 |
| **直方图(Histogram)**             | PostgreSQL数据库使用等频直方图来描述一个列中的数据的分布，高颇值不会出现在直方图中，这样保证数据分布相对平坦 |
| 相关系数                          | 相关系数记录的是当前列未排序的数据分布和排序后的数据分布的相关性，这个值通常在索引扫描时用来估计代价，假设一个列未排序和排序之后的相关性是0，也就是完全不相关，那么索引扫描的代价就会高一些 |
| 类型高频值                        | 用于数组类型或者一些其他类型，PostgreSQL数据库提供了`ts_typanalyze`系统函数来负责生成这种类型的统计信息 |
| 数组类型高频值                    | 用于给数组类型生成直方图,PG提供了`array_typanalyze`系统函数来负责生成这种类型的统<br/>计信息 |
| Range类型基于长度的直方图统计信息 | 用户可以自定义Range类型，PG提供了`range_typanalyze`系统函数负责生成该类型的统计信息 |
| Range类型基于边界的直方图         | Range类型生基于边界的直方图,这种类型直方图通过`range_typanalyze`系统函数来进行统计 |

`PostgreSQL`使用`PG_STATISTIC`系统表保存单列的统计信息， 如果用户要给某一个表成生统计信息使用`ANALYZE`语句进行统计分析，给该表生成统计信息并且保存在`PG_STATISTIC`系统表中。`PostgreSQL`对每一个属性（列）的统计目前最多只能应用 `STATISTIC_NUM_SLOTS`=5种方法，因此在 PG_STATISTIC 会有 `stakind(l-5)`、 `staop(l-5 )`、` stanumbers[1](1-5)`、`stavalues(1-5)`分别是 槽位。如果`stakind` 不为0 ，那么表明这个槽位有统计信息。

```C
// 列可以应用的统计方法
#define STATISTIC_NUM_SLOTS  5

// 高频值, 表示一个列中出现最频繁的值
#define STATISTIC_KIND_MCV 1 
// 直方图
#define STATISTIC_KIND_HISTOGRAM 2
// 相关系数, 记录的是当前列未排序的数据分布和排序后的数据分布的相关性
#define STATISTIC_KIND_CORRELATION 3
// 类型高频值
#define STATISTIC_KIND_MCELEM 4 
// 数组类型高频值
#define STATISTIC_KIND_DECHIST 5 
```

| 列属性        | 数据类型   | 描述                                                         |
| ------------- | ---------- | ------------------------------------------------------------ |
| `stakindN`    | `int2`     | 表示相应类型的统计信息存储在`pg_statistic` 的槽位，取值为[1-5]。 |
| `staopN`      | `oid`      | 用于生成第N个"槽位"的统计信息的操作符。 比如，一个柱面图槽位会显示`<`操作符，该操作符定义了该数据的排序顺序。<br/>关于PG操作符相关内容详见[pg_operator](http://www.postgres.cn/docs/9.4/catalog-pg-operator.html).oid |
| `stanumbersN` | `float4[]` | 第N个"槽位"的对应类型的统计数值                              |
| `stavaluesN`  | `anyarray` | 第N个"槽位"对应类型的统计值的数组。                          |



<center>
    <div><b>多列统计信息</b></div>
</center>

| 统计信息           | 类型                   | 说明                                                         |
| ------------------ | ---------------------- | ------------------------------------------------------------ |
| N-distinct计数     | `STATS_EXT_NDISTINCT`  | 和单列统计信息中的`staditinct`是类似的,`staditinct`记录的是单列中去掉NULL 值和消重之后的数据量或者比例，`STATS_EXT_NDISTINCT`类型的统计信息则记录的是基于多列的消重之后的数据量 |
| 函数依赖性统计信息 | STATS_EXT_DEPENDENCIES | 计算各个列之间的函数依赖度，通过函数依赖度计算各个列之间的依赖关系，从而得到准确的统计信息 |

PG_STATISTIC_EXT系统表保存多列的统计信息 用户需要显式地使用`CREATE STATISTICS`语句对建多列统计信息，此时只是在系统表中增加统计信息项, 并没有真正做统计分析，只有执行`ANALY` ，而且ANALYZE的表属性满足了多列统计信息的要求，才会生成多列统计信息。

| 名称              | 类型              | 描述                                                         |
| ----------------- | ----------------- | ------------------------------------------------------------ |
| `stxkeys`         | `int2vector`      | 统计了哪些列                                                 |
| `stxkind`         | `char[]`          | 已启用统计类型代码的数组, 有效值为<br/>`d`:用于n-distinct统计信息<br/>`f`:用于函数依赖性统计信息 |
| `stxndistinct`    | `pg_ndistinct`    | N-distinct计数，序列化为`pg_ndistinct`类型                   |
| `stxdependencies` | `pg_dependencies` | 函数依赖性统计信息，序列化为`pg_dependencies`类型            |

```c
#define STATS_EXT_NDISTINCT			'd'
#define STATS_EXT_DEPENDENCIES		'f'
#define STATS_EXT_MCV				'm'
#define STATS_EXT_EXPRESSIONS		'e'
```
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
## 选择率
## 示例

```sql
DELETE FROM student
insert into student values(1, 'zs', 1); 
insert into student values(2, 'ls', 1);
insert into student values(3, 'ww', 1);
insert into student values(4, 'zl', 1);
insert into student values(5, 'zs', 2);
insert into student values(6, 'ls', 2);
insert into student values(7, null, null); 

SELECT * FROM student
SELECT relname, oid, relpages, reltuples
	FROM PG_CLASS
	WHERE relname='student';

-- PG_CLASS含两项统计信息
-- relpages, reltuples分别记录当前表占用多少页面以及多少元组
SELECT relname, oid, relpages, reltuples FROM PG_CLASS
	WHERE relname='student'; -- oid=24691

-- 对student表做统计
ANALYZE student;
-- 查询单列的统计信息
-- staatnum分别对应表的列编号
-- stanullfrac表示NULL值率
-- stawidth表示列平均宽度
-- stadistinct 属性消重后数据的个数或比例
-- stop 统计计算过程中设计的操作符
-- stanumbers 存放统计信息的高频值数组或者存放相关系数
SELECT * FROM PG_STATISTIC WHERE starelid=24691;

-- 对一个表创建多列统计信息
CREATE STATISTICS statxt_student ON sno, sname, ssex FROM student;
-- 生成多列统计信息
ANALYZE student(sno, sname, ssex);
-- 查询多列统计信息
-- stxkind 多列统计类型
SELECT * FROM PG_STATISTIC_EXT WHERE stxname='statxt_student';
```



