现代 DBMS 大多支持并行执行。对于窗口函数，由于各个分区之间的计算完全不相关，我们可以很容易地将各个分区分派给不同的节点(线程)，从而达到**分区间并行**。但是，如果窗口函数只有一个全局分区(无 `PARTITION BY` 子句)，或者分区数量很少、不足以充分并行时，怎么办呢？

[Efficient Processing of Window Functions in Analytical SQL Queries](http://www.vldb.org/pvldb/vol8/p1058-leis.pdf)提出使用线段树(Segment Tree)实现高效分区内并行。



>  
>
>  Our algorithm is optimized for high-performance main memory database systems and has excellent performance on modern multi-core CPUs. We show how to fully parallelize all phases of the operator in order to effectively scale for arbitrary input distributions.

本文提出的算法对高性能主存数据库系统进行了优化，在现代多核CP 上具有出色的性能。本文展示了如何完全并行化运算符的所有阶段，以便有效地扩展任意输入分布。



利用线段树数据结构进行窗口函数求值，窗口运算符的设计针对高性能内存数据库进行了优化，例如Hyper。随着具有数十个内核的CPU的商品服务器变得越来越广泛，并行化变得越来越重要。本文的算法被设计为具有高度可扩展性：而不是仅支持分区间并行性，实现起来很简单，但并不适用于所有查询，本文展示如何并行化我们算法的所有阶段。同时，尽可能使用低开销、基于分区的并行化。因此，本文的实现速度快且即使对于没有分区子句的查询和任意的，甚至是高度倾斜，输入分布，也可以扩展。

# 窗口算子

窗口函数处理的涉及以下几个阶段：

1. Partitioning：根据`partition by`属性对输入关系进行分区。

2. Sorting：根据`order by`属性对每个分区进行排序。

3. 窗口函数计算：对每个tuple：

   ​	(a) 计算窗口frame：确定窗口范围(分区子集)。

   ​	(b) 窗口函数求值：在窗口范围内对窗口函数求值。

本节讨论1,2两个阶段，第3个阶段在下节讨论。

> In single-threaded execution, it is usually best to first fully partition the input data using a hash table. With parallel execution,a concurrent hash table would be required for this approach. We have found, however, that concurrent, dynamically-growing hash tables (e.g., split-ordered lists [23]) have a significant overhead in comparison with unsynchronized hash tables. The sort-based approach, without partitioning first, is also very expensive. Therefore, to achieve high scalability and low overhead, we use a hybrid approach that combines the two methods.

在单线程执行中，通常最好先使用哈希表对输入数据进行完全分区。并行执行需要并发哈希表。本文发现，与非同步哈希表相比，并发的、动态增长的哈希表（例如，split-ordered lists[23]）具有显著的开销。基于排序的方法，没有先分区，也非常昂贵。为了实现高可扩展性和低开销，本文采取两种方法结合。

##  Pre-Partitioning into Hash Groups

## Inter- and Intra-Partition Parallelism



# 窗口函数求值



## 聚合算法



Naıve Aggregation

```sql
SUM(b) over (order by a rows between unbounded preceding and current row)
```

---

累计聚合(Cumulative Aggregation)

---

Removable Cumulative Aggregation

---

 Segment Tree Aggregation

---

算法选择

---

