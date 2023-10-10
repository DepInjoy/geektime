# Join策略

分布式系统实现JOIN操作的常见策略有4种：Shuffle Join、Bucket Shuffle Join、Broadcast Join和Colocate Join。

## Shuffle Join

Shuffle Join适合连接的两张表数据量基本相等的场景，首先将两张表中的数据按照关联字段的哈希值打散，使Key值相同的数据分配到同一个节点，然后按照Join算法进行数据关联，最后将结果返回汇总节点

```sql
SELECT * FROM A, B WHERE A.column = B.column;
```



## Bucket Shuffle Join

Bucket Shuffle Join是针对Shuffle Join的一种优化。当连接的两张表拥有相同的分布字段时，我们可以将其中数据量较小的一张表按照大表的分布键进行数据重分布

```sql
SELECT * FROM A, B WHERE A.distribute key = B.any column;
```



## Broadcast Join

Broadcast Join适合大表关联小表的场景，将小表数据复制到所有大表有数据的节点，然后用大表的部分数据关联小表的全部数据，最后将结果返回汇总节点

```sql
SELECT * FROM A, B WHERE A.column = B.column;
```



## Colocate Join

Colocate Join也叫Local Join，是指多个表关联时没有数据移动和网络传输，每个节点只在本地进行数据关联，然后将关联结果返回汇总节点。

```sql
SELECT * FROM A, B WHERE A.colocate column = B.colocate column;
```



---

相比Shuffle Join和Broadcast Join，Colocate Join在查询时没有数据移动和网络传输，性能会更高。Colocate Join相比Shuffle Join可以拥有更高的并发度，也可以显著提升关联性能。Doris的Colocate Join，默认关闭，开启Colocate Join只需要在FE配置中设置`disable_colocate_join`为false。为了在Doris中实现Colocate Join，我们需要针对表进行一些特殊的设置。

1. Colocate Join的表必须是OLAP类型的表。
2. Colocate Join的表必须设置相同的colocate_with属性。
3. Colocate Join的表的BUCKET数必须一样。
4. Colocate Join的表的副本数必须一样。
5. Colocate Join的表的Distributed Columns的字段必须一样并且是关联字段中的一个。

# BitMap精准去重

用户在使用Doris进行精准去重分析时，通常会有两种方式。

1. 基于明细去重：传统的COUNT DISTINCT方式，优点是可以保留明细数据，提高分析灵活性。缺点是需要消耗极大的计算和存储资源，对大规模数据集和查询延时敏感的去重场景不够友好。
2. 基于预计算去重：这种方式也是Doris推荐的方式。在某些场景中，用户可能不关心明细数据，仅仅希望知道去重后的结果。在这种场景下，用户可以采用预计算的方式进行去重分析，本质上是利用空间换时间，也是MOLAP聚合模型的核心思路，就是将计算提前到数据导入过程，减少存储成本和查询时的计算成本，并且使用ROLLUP表降维方式，进一步减小现场计算的数据集大小。

Doris是基于MPP架构实现的，在使用COUNT DISTINCT做精准去重时，可以保留明细数据，灵活性较高；但是，在查询过程中需要进行多次数据重分布，会导致性能随着数据量增大而直线下降。

Doris中的BITMAP技术采用Roaring Bitmap（高效压缩位图，简称为RBM，是普通Bitmap的进化）实现。对于稀疏的BITMAP，存储空间显著降低。BITMAP去重涉及的计算包括对给定下标的bit置位，统计BITMAP的bit置位0和1的个数，分别为O(1)操作和O(n)操作，并且后者可使用clzL、ctz等指令高效计算。此外，BITMAP去重在MPP执行引擎中还可以并行处理，每个计算节点各自计算本地子BITMAP，使用bitor操作（位或操作，即比特位的或计算）将这些子BITMAP合并成最终的BITMAP。bitor操作比基于Sort和基于Hash的去重效率要高，无条件依赖和数据依赖，可向量化执行。

<b>需要注意，BITMAP INDEX和BITMAP去重都是利用的BITMAP技术，但引入动机和解决的问题完全不同，前者用于低基数的枚举型列的等值条件过滤，后者用于计算一组数据行的指标列的不重复元素的个数。</b>

BITMAP列只能存在于聚合表中。创建表时，用户可指定指标列的数据类型为BITMAP，指定聚合函数为BITMAP_UNION。当在BITMAP类型列上使用COUNT DISTINCT时，Doris会自动转化为BITMAP_UNION_COUNT计算。

引入BITMAP类型的数据有两种场景：

1. 导入数据或者插入数据的时候直接将数据转换成BITMAP类型。
2. 在Doris数据库内部将字段转换成BITMAP类型。



# HLL近似去重

在现实场景中，随着数据量的增大，对数据进行去重分析的压力会越来越大。当数据规模大到一定程度时，精准去重的成本会比较高。HyperLogLog（简称HLL）是一种近似去重算法，它的特点是具有非常优异的空间复杂度$O(mloglogn)$，时间复杂度为$O(n)$，计算结果误差可控制在1%～10%内，且误差与数据集大小以及所采用的哈希函数有关，它能够使用极少的存储空间计算一个数据集的不重复元素的个数。

```sql
CREATE TABLE test_hll(
	dt 	DATE,
    id	INT,
    uv	HLL	HLL_UNION
) AGGREGATE KEY(dt, id)
DISTRIBUTED BY HASH(ID) BUCKETS 1
PROPERTIES(
	"replication_num" = "1",
    "storage_format" = "DEFAULT"
);

-- 数据量很大时,最好为高频率HLL查询建立对应的ROLLUP表
ALTER TABLE test_hll ADD ROLLUP hll_pv(dt, id);
-- 之后导图数据，就可以开始查询
```

```sql
-- 求总uv
SELECT HILL_UNION_AGG(uv) FROM test_hll;

-- 等价于
SELECT COUNT(DISTINCT uv) FROM test_hll;
```



#  GROUPING SETS多维组合

GROUPING SETS最早出现在Oracle数据库中，是多维数据分析中比较常见的一个数据汇总方式，支持提前按照各种指定维度预聚合数据，通过扩展存储来提高特定组合条件下的数据查询性能。GROUPING SETS是对GROUP BY子句的扩展，能够在一个GROUP BY子句中一次实现多个集合的分组。其查询效果等价于将多个GROUP BY子句进行UNION操作。特别地，一个空的子集意味着将所有的行聚集成一条记录。

```sql
SELECT k1, k2, SUM(k3) FROM t
	GROUP BY GROUPING SETS ((k1, k2), (k1), (k2), ());
	
-- 等价于
SELECT k1, k2, SUM(k3) FROM t GROUP BY k1, k2
UNION ALL
SELECT k1, k2, SUM(k3) FROM t GROUP BY k1
UNION ALL
SELECT k1, k2, SUM(k3) FROM t GROUP BY k2
UNION ALL
SELECT k1, k2, SUM(k3) FROM t;
```



GROUPING SETS还有两个扩展子句：`ROLLUP`和`CUBE`，区别在于`ROLLUP`子句只会逐步减少分组字段，`CUBE`子句则是穷举所有可能的分组组合字段。

```sql
SELECT visit_date, page_id, count(distinct user_id) as uv, sum(visit_cnt) as pv
	FROM page_visit_detail
	GROUP BY ROLL UP(visit_date, page_id);
	
-- 等价于
SELECT visit_date, page_id, count(distinct user_id) as uv, sum(visit_cnt) as pv
	FROM page_visit_detail
	GROUP BY GROUPING SETS ((visit_date, page_id), (page_id), ());
```



```sql
SELECT visit_date, page_id, count(distinct user_id) as uv, sum(visit_cnt) as pv
	FROM page_visit_detail
	GROUP BY CUBE(visit_date, page_id);
	
-- 等价于
SELECT visit_date, page_id, count(distinct user_id) as uv, sum(visit_cnt) as pv
	FROM page_visit_detail
	GROUP BY GROUPING SETS ((visit_date, page_id), (visit_date), (page_id), ());
```



# 索引

目前，Doris主要支持两类索引：一类是内建的智能索引，包括前缀索引和ZoneMap索引；另一类是用户创建的二级索引，包括Bloom Filter索引和BITMAP索引。其中，ZoneMap索引是在列存储格式上对每一列索引信息自动维护，包括Min、Max、Null值个数等。这种索引对用户透明，且无法优化和调整。



## 前缀索引

本质上，Doris的数据存储在类似SSTable（Sorted String Table）的数据结构中。该结构是一种有序的数据结构，可以按照指定的列进行排序存储。在这种数据结构上，以排序列为条件进行查找非常高效。
在Aggregate、Unique和Duplicate三种数据模型中，底层的数据存储是对各自建表语句中AGGREGATE KEY、UNIQUE KEY和DUPLICATE KEY指定的列进行排序存储的。
前缀索引是一种在排序的基础上根据给定前缀列，快速查询数据的索引方式。前缀索引是以Block为粒度创建的稀疏索引，一个Block包含1024行数据，每个Block以第一行数据的前缀列的值为索引。建表时，建议将查询中常见的过滤字段放在Schema的前面，区分度越大，查询频次越高的字段越往前放。



## BF索引

用户可以在建表时指定在某些列上创建Bloom Filter索引（以下简称BF索引），也可以在运行时通过ALTER TABLE命令新增BF索引。建表时，BF索引创建是通过在建表语句的PROPERTIES里加上bloom_filter_columns参数来实现的，支持多个字段组合。

BF索引也是以Block为粒度创建的。每个Block中指定列的值作为一个集合生成一个BF索引条目，用于在查询时快速过滤不满足条件的数据。BF索引比较适合创建在高基数列上，比如UserID，如果创建在低基数列上，比如“性别”列，每个Block几乎都会包含所有取值，导致索引失去意义。



## BitMap索引

用户可以在建表时指定在某些列上创建BITMAP索引，也可以在建表后通过ALTER TABLE或者CREATE INDEX命令新增BITMAP索引。

BITMAP索引是一种特殊的数据库索引技术，使用bit数组进行数据存储。位置编码中的每一位表示键值对应的数据行的有无。一个位图指向的是几十甚至成百上千行数据的位置。
相对于B*Tree索引，BITMAP索引占用的空间非常小，创建和查询速度非常快。当根据键值做查询时，数据库可以根据BITMAP索引快速定位到具体的行。而当根据键值做and、or或in查询时，数据库直接用索引的位图进行与、或运算，快速获得结果行数据。
Doris中的BITMAP索引仅支持在单列上创建，并且只支持定长类型的字段查询，不支持TEXT或者STRING类型的字段查询。BITMAP索引适合低基数列查询场景，建议在100到100000之间，如职业、地市等。基数太高，BITMAP索引则没有明显优势；基数太低，BITMAP索引空间效率和性能会大大降低。

# 物化视图

物化视图是物理化的视图，即保存查询结果数据的视图。物化视图结合了表和视图的优点，既支持动态刷新数据，又可以满足高效查询需求。
物化视图是一种以空间换时间的数据分析技术。Doris支持在基础表之上建立物化视图，比如可以在明细数据模型表上基于部分列建立聚合视图，这样可以同时满足对明细数据和聚合数据的快速查询。同时，Doris能够保证物化视图和基础表的数据一致性，并且在查询时自动匹配合适的物化视图，极大地降低数据维护成本，为用户提供一致且透明的查询服务

```sql
ATE MATERIALIZED VIEW [MV name]
	AS [query] [PROPERTIES ("key" = "value")]
```

物化视图的查询有以下限制。

- 查询涉及的所有列只允许使用一次。
- 仅支持不带表达式计算的单列查询，不支持CASE WHEN处理，也不支持函数处理。
- GROUP BY子句非必需。
- ORDER BY子句指定列的顺序必须和查询字段顺序一致。
- 如果不声明ORDER BY，Doris根据规则自动补充排序列。如果物化视图是聚合类型，所有的分组列自动补充为排序列。如果物化视图是非聚合类型，前36 B自动补充为排序列。如果自动补充的排序列小于3个字段，前3个字段作为排序列。
- 查询的基础表必须是单表，且不能放在子查询中。

# ROLLUP

ROLLUP在多维分析中是“上卷”的意思，即将数据按某种指定的粒度进行进一步聚合。Base表中保存着按用户建表语句指定的方式存储的基础数据。在Base表之上，我们可以创建任意多个ROLLUP表。这些ROLLUP表中的数据是基于Base表产生的，并且在物理上是独立存储的。ROLLUP表的基本作用是在Base表的基础上，获得更粗粒度的聚合数据。

ROLLUP最根本的作用是提高某些查询的效率（无论通过聚合来减少数据量，还是修改列顺序以匹配前缀索引）。因此，ROLLUP的含义已经超出“上卷”的范围。这也是源代码中将其命名为Materialized Index（物化索引）的原因。

- ROLLUP表附属于Base表，可以看作是Base表的一种辅助数据结构。用户可以在Base表的基础上，创建或删除ROLLUP表，但是不能在查询中显式地指定查询某ROLLUP表。是否命中ROLLUP表完全由Doris查询引擎决定
- ROLLUP表中的数据是独立存储的。因此，创建的ROLLUP表越多，占用的磁盘空间也就越大，同时对导入速度也会有影响，但是不会降低查询效率。
- ROLLUP表中的数据更新与Base表中的数据更新是完全同步的，无须用户关心这个问题。
- ROLLUP表中列的聚合方式与Base表中的完全相同。用户在创建ROLLUP表时无须指定，也不能修改。
- 查询引擎能否命中ROLLUP表的一个必要条件是，查询所涉及的所有列（包括select list和where中的查询条件列等）都存在于该ROLLUP表的列中。否则，查询引擎只能命中Base表。
- 某些类型的查询（如count（*））在任何条件下，都无法命中ROLLUP表。
- 可以通过EXPLAIN your_sql命令获得查询执行计划，在执行计划中，查看是否命中ROLLUP表。

# 向量化查询引擎

向量化其实就是由一次对一个值进行运算，转化成一次对一组值进行运算。从CPU角度分析，现代CPU都支持将单个指令应用于多个数据的SIMD向量化计算。所谓向量化计算，就是利用SIMD指令进行运算，比如一个具有128位寄存器的CPU，可以一次性从内存中拉取4个32位数据，并且进行计算，再把该128位数据写回内存，这比一次执行一条指令快4倍。



