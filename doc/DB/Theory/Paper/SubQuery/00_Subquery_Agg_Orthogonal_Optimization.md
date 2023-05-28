# 标准子查询执行策略

SQL中有两种聚合形式，这两种聚合行为在空输入处理有所不同

1. 矢量聚合(Vector Aggregation)：指定分组列和Agg来计算，DISTINCT去除重复是一种是矢量聚合的一种特殊情况，即将相等的值折叠成一行，但实际上没有聚合函数来计算，Microsoft SQL Server将`DISTINCT`规范化(Normalize)为`GroupBy`。

    ```sql
    -- 计算每天的销售总额
    SELECT o_orderdate, SUM(o_totalprice)
    	FROM orders
    	GROUP BY o_orderdate;
    ```

    如果`orders`为空，则查询计算结果也为空。

2. 标量聚合(Scalar Aggregation)：不指定分组列，例如

    ```sql
    -- 计算总销售额,结果只有一行
    SELECT SUM(o_totalprice) FROM orders;
    ```

    如果`orders`为空，对于`SUM`计算结果是`NULL`，对于`COUNT`结果是0。

在代数表达式中，采用$\mathcal{G}_{A,F}$表示矢量聚合，$\mathcal{G}_{F}^{1}$表示标量聚合。



对于下面的查询：找出消费大于100000的客户，它是相关子查询(Correlated Subquery)：

遍历`customer`客户表，对每个客户在`orders`消费表中求相应客户消费总额度。

```sql
SELECT c_custkey
	FROM customer
	WHERE 1000000 < (
        -- 每个客户的消费总额度，scalar聚合
        SELECT SUM(o_totalprice) FROM orders
        	WHERE o_custkey = c_custkey);
```

1. **相关执行(Correlated Execution)**。最直接的执行算法，类似于`NestLoop`，先获取每个客户并计算子查询指定的总金额，然后过滤出订购金额少于指定金额的客户。也就是，先用外层查询`customer`的每行去遍历子查询内层查询的orders表(对于外层查询的一行数据`c_custkey`已知，计算得到客户的消费总额(`SELECT SUM(o_totalprice) FROM orders WHERE o_custkey = c_custkey`)，一行数据即Scalar Agg)，如果这个客户的消费总额`> 1000000`，则输出这一行的`customer.c_custkey`。

    这种执行策略性能低下，它是逐行处理`customer`，而不是面向集合的执行(set oriented execution)，其复杂度是笛卡尔积。在外层表很小且存在适当的索引的情况下，可能是最好的策略。。

2. 先`Outer Join`然后`Agg`：该执行策略最早由Dayal提出。使用set-oriented算法，先收集customer的所有orders，之后在`customer`分组，再进行聚合。

    ```sql
    SELECT c_custkey
    	-- 1. 先Left Outer Join, 建立所有customer的order信息
    	-- 采用LOJ即使customer无法和orders不匹配也会被保留，对于不匹配的行agg结果为NULL
    	-- 这里如果聚集函数是COUT(*)先Join再agg不对，需要将其转换COUNT(column)
    	FROM customer LEFT OUTER JOIN orders ON o_custkey = c_custkey
    	-- 2. 再分组并聚集
    	GROUP BY c_custkey
    	HAVING SUM(o_totalprice) > 1000000;
    ```

3. 先`AGG`再Join。该策略最早由Kim提出。先对`orders`分组并聚合计算(`GROUP BY + HAVING`),再和`customer`进行Join，相当于将Agg下推导Join的下面。SQL公式(formulation)采用派生表(derived table)，而不是子查询。

    ```sql
    -- 2. 再Join
    SELECT c_custkey FROM customer, (
        	-- 1. 先对orders按照c_custkey分组并聚合计算
            SELECT o_custkey FROM orders
    			GROUP BY c_custkey
    			HAVING SUM(o_totalprice) > 1000000)
        WHERE o_custkey = c_custkey;
    ```

# 基于原语正交组合的优化策略

上面提出的这些，最有效的执行策略最终取决于表大小，选择率，Agg压缩率，需要根据代价动态选择。本文提出采用正交的、可重用的基元来实现上面列出的功能甚至更多，并使用代价估计在它们之间选择较优的执行策略。

<center>
    <img src="./img/Figure_1.png">
</center>

上图展示了原始优化如何实现不同的子查询策略。

> By implementing all these orthogonal techniques, the query processor should then produce the same efficient execution plan for the various equivalent SQL formulations we have listed above, achieving a degree of syntax-independence.

通过实现所有这些正交技术，查询优化可以列出上面提到的各种等价SQL公式(formulation)为其生成等价的有相同执行效率的执行计划，实现一定程度的语法独立性。

上图展示的基本变换描述如下：

1. `Algebrize into initial operator tree`
2. `Remove correlations`
3. `Simplify outerjoin`
4. `Reorder GroupBy`

引入表示子查询的逻辑算子Apply(也称作Correlated Join)，接受两个关系树输入，与一般Join不同的是，Apply的Inner输入是带参数的关系树，即`E`代表参数化的子查询。该

引入Apply算子使用关系代数来描述关联子查询，其语义：
$$
\begin{array}{l}
R \ A^{\otimes} \ E = \bigcup_{r\in R}(\{r\} \otimes E(r))
\end{array}
$$
对关系`R`中每行数据`r`，执行表达式`E`，返回符合条件的`r`集合。Apply算子适用于Scalar Valued，将标量作为子查询的参数。

Segment Apply算子：table-valued做为子查询的参数。
$$
R \ \mathcal{S}\mathcal{A}_A \ E = \bigcup_{a}({a} \times E(\sigma_{A = a} \ R))
$$
关系R做为输入，使用某个列A对R进行分组(类似`group by A`)，将一个个分组S做为参数，传给`E`。这

通过引入Apply算子，以关系代数的方式形式化子查询，在原有关系代数的基础上增加了SQL执行的可选择空间。普通的关系代码描述的集合操作，Apply算子和普通关系代数算子不同的是：Apply是一次执行一个tuple，tuple-at-a-time。



无需在执行器中实现Apply算子，可以通过9中规则消除Apply算子转换成Join算子，其方法是：

1. 先用Apply算子替换子查询

2. Apply算子下推(相当于`Agg`，`Filter`上移)，直到右表不再包含左表的参数位置

3. 最终得到醉花间的Apply算子形式。

    

$$
\begin{array}{l}

R \ A^{\otimes}\  E = R \otimes_{true} E,当E不包含R的列时 && (1)\\
R \ A^{\otimes}\  (\sigma_p E) = R \otimes_p E, 当E不包含R的列时 && (2)\\
\\ 
\\
R \ A^{\times}\ (\sigma_p E) = \sigma_p(R \ A^{\times}\ E)  && (3)\\
R \ A^{\times}\ (\pi_v E) = \pi_{v \cup cols(R)}(R \ A^{\times}\ E) && (4)
\\
\\
R \ A^{\times}\ (E_1 \cup E_2) = (R \ A^{\times}\ E_1) \cup (R\ A^{\times}\ E_2) && (5)\\
R \ A^{\times}\ (E_1 - E_2) = (R \ A^{\times}\ E_1) - (R\ A^{\times}\ E_2) && (6) \\
R \ A^{\times}\ (E_1 \times E_2) = (R \ A^{\times}\ E_1) \Join_{R.key} (R\ A^{\times}\ E_2) && (7)\\
\\
\\
R \ A^{\times}\ (\mathcal{G}_{A, F}E) = \mathcal{G}_{A \cup pk(R), F}(R \ A^{\times}\ E)  && (8)\\
R \ A^{\times}\ (\mathcal{G}^{1}_{F}E) = \mathcal{G}_{A \cup pk(R), F^{'}}(R \ A^{LOJ}\ E)  && (9)\\

其中\\
\Join_{R.key}表示按照R的可用做自然连接 \\
\otimes可以是\left \{ \times, LOF, \exists, \nexists \right \}中的任意一个

\end{array}
$$



通过上述的方法把任意子查询去关联化，比如：对于子查询中包含`EXISTS`, `NOT EXISTS`，可以转换成`Scalar COUNT`聚合，是否大于0。



```sql
SELECT c_name, (
        SELECT o_orderkey FROM orders WHERE o_custkey = c_custkey)
	FROM customer
```

上面的SQL会返回3种可能的结果：

1. 返回`null`，那么该子查询最终结果返回`null`
2. 返回一行，那么该子查询最终结果返回该行
3. 返回多行，那么需要产生run-time-error。

引入`Max1Row`算子，对子查询结果集检查，如果超过1行报run-time-error。
$$
\begin{array}{l}
  Max1Row(E) = \left\{\begin{matrix} 
  &Null, &if |E| = 0 \\ 
  &E, &if|E| = 1 \\ 
  &Error,&otherwise 
\end{matrix}\right.    
\end{array}
$$


并不是所有的子查询都需要`Max1Row`判断：

```sql
-- c_custkey是customer的键,是唯一的,不会产生多行
SELECT o_orderkey, (
    	SELECT c_name FROM customer WHERE c_custkey = o_custkey)
	FROM orders
```



Conditional Scalar Execution

```sql
-- 当COND为true,VALUE2不应该被执行, 也需要产生run-time-error 没太理解
CASE WHEN <COND> THEN <VALUE1> ELSE <VALUE2> END
```

通过扩展Apply with conditional来实现



Local Aggreagte

下推部分`group by`的工作量到下层，聚合函数f被拆分成多个聚合函数：

1. 下层的称为`Local Agg`：(先对右表进行一次细粒度聚合，比如`count(*)`，一定程度上减少了结果集)
2. 上层的称为`Global Agg`：（在join后，进行一次最终的聚合，比如`sum(*)`）

$$
f(\bigcup_{i=1}^{n}S_i) = f_g(\bigcup_{i=1}{n} \ f_l(S_i))
$$

## Segmented Execution

`SegmentApply`和`Apply`的区别是：参数并不是S中的一行，而是一个集合。

找出`lineitem`中`quantity`小于平均的20%。子查询的结果并不需要返回具体的平均值X，它仅仅是用来过滤。

```sql
SELECT l_partkey, l_extendedprice 
    FROM lineitem, (
        -- 按照lineitem.l_partkey进行分组,得到分组S
        SELECT l_partkey AS l2_partkey, 0.2 * AVG(l_quantity) AS x
            FROM lineitem
            GROUP BY l_partkey) AS aggresult 
    WHERE l_partkey = l2_partkey AND l_quantity < x
```



SQL Server中优化器的三个阶段：

1. Parser And Bind阶段：将SQL文本翻译成Operator Tree，此时含子查询。
2. Query normalization阶段：(逻辑优化)简化Operator Tree：Outer Join转Join；消除空表达式；递归消除多层关联/非关联子查询。

3. Cost-based optimization阶段：(物理优化)，join顺序；上拉/下推`group by`和`join`；基于Volcano优化器，通过一些transformation rule来扩展搜索空间；

