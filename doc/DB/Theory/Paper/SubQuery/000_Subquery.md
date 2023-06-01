# 子查询的分类

根据子查询中涉及的关系对象与外层关系对象间的关系，子查询可以分为：

- 相关子查询。子查询的执行依赖于外层父查询的一些属性值。

    ```sql
    SELECT * FROM t1 WHERE col_1 = ANY(
        -- 子查询中存在父查询的t1表的col_2列
        SELECT col_1 FROM t2 WHERE t2.col_2 = t1.col_2);
    ```

- 非相关子查询，子查询的执行不依赖于外层父查询的任何属性值，这样的子查询具有独立性，可独自求解，形成一个子查询计划先于外层的查询求解。

    ```sql
    SELECT * FROM t1 WHERE col_1 = ANY (
        -- 子查询中(t2)不存在父查询(t1)的属性,可以独立执行
        SELECT col_1 FROM t2 WHERE t2.col_2 = 10);
    ```

根据产生的数据，可以将子查询划分为以下几类：

- 标量(Scaler-Valued)子查询:输出只有一行一列的结果表。如果结果为空(0行)，输出NULL，如果输出超过一行会产生运行异常。它可以出现在任何包含标量的地方，例如SELECT、WHERE等子句中。
- **存在性检测（Existential Test）** 子查询：特指 EXISTS 的子查询，返回一个布尔值。如果出现在 WHERE 中，这就是我们熟悉的 Semi-Join。它可能出现在任何可以放布尔值的地方。
- **集合比较（Quantified Comparision)** 子查询：特指 IN、SOME、ANY 的查询，返回一个布尔值，常用的形式有：`x = SOME(Q)` （等价于 `x IN Q`）或 `X <> ALL(Q)`（等价于 `x NOT IN Q`）。它可能出现在任何可以放布尔值的地方。

从结果集，子查询可以分为：

- 标量子查询(`Scalar SubQuery`)。子查询返回的结果集类型是一个单一值。
- 列子查询。子查询返回的结果集类型是一条单一元组(a single row)。
- 行子查询。子查询返回的结果集类型是一个单一列(return a single column)。
- 表子查询。子查询返回的结果集类型是一个表(多行多列)(return a table，one or more rows of one or more columns)。

# 子查询优化思路

首先，为什么要做子查询优化？

在数据库实现早期，查询优化器对子查询一般采用嵌套执行的方式，即对父查询中的每一行，都执行一次子查询，这样子查询会执行很多次。这种执行方式效率很低。而对子查询进行优化，可能带来几个数量级的查询效率的提高。子查询转变成为连接操作之后，会得到如下好处：

- 子查询不用执行很多次。
- 优化器可以根据统计信息来选择不同的连接方法和不同的连接顺序。
- 子查询中的连接条件、过滤条件分别变成了父查询的连接条件、过滤条件，优化器可以对这些条件进行下推，以提高执行效率。

---

那么，需要进行子查询优化，有哪些可用的技术？

1. <b><font color="orange">子查询合并(Subquery Coalescing)</font></b>。在某些条件下(语义等价：两个查询块产生同样的结果集)，多个子查询能够合并成一个子查询(合并后还是子查询，以后可以通过其他技术消除子查询)。这样可以把多次表扫描、多次连接减少为单次表扫描和单次连接.例如：

    ```sql
    SELECT * FROM t1 WHERE a1＜10 AND (
        EXISTS (SELECT a2 FROM t2 WHERE t2.a2＜5 AND t2.b2=1) OR
        EXISTS (SELECT a2 FROM t2 WHERE t2.a2＜5 AND t2.b2=2)
    );
    
    -- 优化为
    SELECT * FROM t1 WHERE a1＜10 AND (
        -- 将两个EXISTS合并为一个,条件也合并
        EXISTS (SELECT a2 FROM t2 WHERE t2.a2＜5 AND (t2.b2=1 OR t2.b2=2)
    );
    ```

2. <b><font color="orange">子查询展开(Subquery Unnesting)</font></b>。又称子查询反嵌套，又称为子查询上拉。其实质是把某些子查询重写为等价的多表连接操作。带来的好处是，有关的访问路径、连接方法和连接顺序可能被有效使用，使得查询语句的层次尽可能地减少。

    ```sql
    SELECT * FROM t1, (SELECT * FROM t2 WHERE t2.a2 ＞10) v_t2
    	WHERE t1.a1＜10 AND v_t2.a2＜20;
    	
    -- 优化为
    SELECT * FROM t1, t2
    	WHERE t1.a1<10 AND t2.a2<20 AND t2.a2>10;
    ```

3. <b><font color="orange">聚集子查询消除(ggregate Subquery Elimination)</font></b>聚集函数上推，将子查询转变为一个新的不包含聚集函数的子查询，并与父查询的部分或者全部表做左外连接。通常，一些系统支持的是标量聚集子查询消除。

    ```sql
    SELECT * FROM t1 WHERE t1.a1 > (SELECT avg(t2.a2) FROM t2);	
    ```

4. <b><font color="orange">利用窗口函数消除子查询的技术(Remove Subquery using Window functions，RSW)</font></b>

5. <b><font color="orange">Push SubQurery</font></b>

6. 其他。

# 参考资料
1. 数据库查询优化器的艺术：原理解析与SQL性能优化
