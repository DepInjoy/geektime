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

Doris中的BITMAP技术采用Roaring Bitmap（高效压缩位图，简称为RBM，是普通Bitmap的进化）实现。对于稀疏的BITMAP，存储空间显著降低。BITMAP去重涉及的计算包括对给定下标的bit置位，统计BITMAP的bit置位0和1的个数，分别为O(1)操作和O(n)操作，并且后者可使用clzL、ctz等指令高效计算。此外，BITMAP去重在MPP执行引擎中还可以并行处理，每个计算节点各自计算本地子BITMAP，使用bitor操作（位或操作，即比特位的或计算）将这些子BITMAP合并成最终的BITMAP。bitor操作比基于Sort和基于Hash的去重效率要高，无条件依赖和数据依赖，可向量化执行。需要注意，BITMAP INDEX和BITMAP去重都是利用的BITMAP技术，但引入动机和解决的问题完全不同，前者用于低基数的枚举型列的等值条件过滤，后者用于计算一组数据行的指标列的不重复元素的个数。

BITMAP列只能存在于聚合表中。创建表时，用户可指定指标列的数据类型为BITMAP，指定聚合函数为BITMAP_UNION。当在BITMAP类型列上使用COUNT DISTINCT时，Doris会自动转化为BITMAP_UNION_COUNT计算。

引入BITMAP类型的数据有两种场景：

1. 导入数据或者插入数据的时候直接将数据转换成BITMAP类型。
2. 在Doris数据库内部将字段转换成BITMAP类型。



# HLL近似去重



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

