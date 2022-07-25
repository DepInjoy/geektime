# 聚合算子
聚合操作可划分为两类
- `scalar-agg`,例如`sum`, `avg`, `min`, `max`等的数据聚合操作，最终返回一个数据行的结果集
- `multi-agg`, 对应于SQL中的`GROUP By`

## Stream Aggregate算法
> 原理:Stream Aggregate的计算需要保证输入数据按照Group-By列有序。在计算过程中，每当读到一个新的Group的值或所有数据输入完成时，便对前一个Group的聚合最终结果进行计算。Stream Aggregate的输入数据需要保证同一个Group的数据连续输入，所以Stream Aggregate处理完一个Group的数据后可以立刻向上返回结果[1]

伪码:
```
clear the current aggregate results 
clear the current group by columns 
for each input row 
  begin 
    if the input row does not match the current group by columns 
      begin 
        output the aggregate results 
        clear the current aggregate results 
        set the current group by columns to the input row 
      end 
    update the aggregate results with the input row 
  end
```
## Hash Aggregate算法
> 在Hash Aggregate 的计算过程中，我们需要维护一个Hash表，Hash表的键为聚合计算的 Group-By 列，值为SQL的输出数据。计算过程中，只需要根据每行输入数据计算出键，在 Hash 表中找到对应值进行更新即可。
> 
> 由于分布式计算的需要，对于聚合函数的计算阶段进行划分，相应定义了两种计算模式：
> - CompleteMode
> - FinalMode，PartialMode[1]。

|计算模式 | 输入值 |输出值|
|:---:|:----:|:----:|
|CompleteMode|原始数据最终结果|
|FinalMode| 中间结果 | 最终结果|
| PartialMode | 原始数据 | 中间结果|

![](./img/Hash-Agg-Cal-Mode.png)

```
begin 
  calculate hash value on group by column(s) 
  check for a matching row in the hash table 
  if we do not find a match 
    insert a new row into the hash table 
  else 
    update the matching row with the input row 
end 
output all rows in the hash table
```

# Join算子(关联)
常见的关联算法有三大类，分别是嵌套循环(Nested Loop Join)、排序归并(Sort-Merge Join)和哈希(Hash Join).
## 嵌套循环关联算法
嵌套循环算法又可以细分为三种，分别是 Simple Nested-Loop Join(SNLJ)、Block Nested-Loop Join(BNJ)和Index Lookup Join(ILJ).
### Simple Nested-Loop Join
SNLJ 的执行过程是这样的：
- 遍历 Outer 表，取一条记录 r1
- 遍历 Inner 表，对于 Inner 表中的每条记录，与 r1 做 join 操作并输出结果
- 重复步骤 1 和 2，直至遍历完 Outer 表中的所有数据，就得到了最后的结果集。

SNLJ算法简单，却存在性能问题. 每次为了匹配Outer表的一条记录，都要对Inner表做一次全表扫描操作。而全表扫描的磁盘 I/O 开销很大。

### Block Nested-Loop Join
BNJ是对SNLJ的一种优化，改进点是减少Inner表的全表扫描次数。BNJ的变化主要在于步骤1，读取Outer表时不再只取一条记录，而是读取一个批次的x条记录，加载到内存中。这样执行一次Inner表的全表扫描就可以比较x条记录。在MySQL中，这个x对应Join Buffer的设置项，它直接影响了BNJ的执行效率。

与SNLJ相比，BNJ虽然在时间复杂度都是`O(m*n), m和n分别是Outer表和Inner表的记录行数`，但磁盘I/O的开销却明显降低了.

### Index Lookup Join
Index Lookup Join（ILJ）就是在 BNJ 的基础上使用了索引，算法执行过程是这样的：
- 遍历 Outer 表，取一个批次的记录 ri
- 通过连接键(Join Key)和ri可以确定对Inner表索引的扫描范围，再通过索引得到对应的若干条数据记录，记为sj
- 将ri的每一条记录与sj的每一条记录做Join操作并输出结果
- 重复前三步，直到遍历完Outer表中的所有数据，就得到了最后结果集。

## 归并排序算法
也就是Sort-Merge Join(SMJ)，也被称为Merge Join。SMJ 可以分为排序和归并两个阶段：
- 第一阶段是排序，就是对 Outer 表和 Inner 表进行排序，排序的依据就是每条记录在连接键上的数值。
- 第二阶段就是归并，因为两张表已经按照同样的顺序排列，所以 Outer 表和 Inner 表各一次循环遍历就能完成比对工作了。

如果表的连接键刚好是有序的，那么SMJ就是三种嵌套循环算法中成本最低的，时间复杂度只有`O(m+n)`。

## Hash关联
Hash关联不仅能够处理大小表关联，对提升大表之间关联的效率也有明显效果，但只适用于等值连接。
哈希连接的基本思想是取关联表的记录，计算连接键上数据项的哈希值，再根据哈希值映射为若干组，然后分组进行匹配。这个算法体现了一种分治思想。具体来说，常见的哈希连接算法有三种，分别是 Simple Hash Join、Grace Hash Join 和 Hybrid Hash Join。

### Simple Hash Join
也称为经典哈希连接(Classic Hash Join)，它的执行过程包括建立阶段(Build Phase)和探测阶段(Probe Phase)。

### Grace Hash Join
GHJ算法与SHJ的不同之处在于，GHJ正视了哈希表大于内存这个问题，将哈希表分块缓存在磁盘上。GHJ中的Grace并不是指某项技术，而是首个采用该算法的数据库的名字。

### Hybrid Hash Join
主要是针对 Grace Hash Join 的优化，在内存够用的情况下，可以将 Inner 表的第一个 Bucket 和 Outer 表的第一个 Bucket 都保留在内存中，这样建立阶段一结束就可以进行匹配，节省了先写入磁盘再读取的两次 I/O 操作。

# 参考资料
- 1 [数据库内核:物理算子之Aggregate聚合算子实现](https://blog.csdn.net/Night_ZW/article/details/109446779)
- 2 [20|关联查询：如何提升多表Join能力?](https://time.geekbang.org/column/article/289299?utm_source=time_web&utm_medium=menu&utm_term=timewebmenu)
