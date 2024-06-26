窗口函数也称为OLAP函数，其中OLAP是Online Analytical Processing的简称，意思是对数据库数据实时分析处理。例如，市场分析、创建财务报表、创建计划等日常性商务工作。

窗口函数语法：
```sql
window_function (expression) OVER (
   [ PARTITION BY expr_list ]
   [ ORDER BY order_list ]
   [ frame_clause ]
)

expr_list:
    expression | column_name [, expr_list ]

order_list:
    expression | column_name [ASC | DESC] [NULLS { FIRST | LAST }] [, order_list]
```
其中，`frame_clause`语法表示为
```sql
frame_clause:
   { RANGE | ROWS } frame_start
   { RANGE | ROWS } BETWEEN frame_start AND frame_end

frame_start可以是下面其一:
    UNBOUNDED PRECEDING
    CURRENT ROW
	value PRECEDING

frame_end可以是下面其一:
    CURRENT ROW
    UNBOUNDED FOLLOWING
	value FOLLOWING
```
`PARTITION BY`在横向上对表进行分组，`ORDER BY`决定了纵向排序的规则，表明分区中的行如何排序，分区中`ORDER BY`相等的行被认为是对等(peers)的，如果没有`ORDER BY`则分区行是无序的，没有隐式的处理顺序，分区中的所有行是对等的。通过`PARTITION BY`分组后的记录集合称为窗口(代表范围)。可以不指定`PARTITION BY`，如果不指定`PARTITION BY`则表示将整个表作为一个大的窗口来计算。

> If PARTITION BY is omitted, there is a single partition consisting of all query rows.
> 也就是说，如果不指定PARTITION BY，则所有查询行看作一个分区。
>
> If ORDER BY is omitted, partition rows are unordered, with no processing order implied, and all partition rows are peers. 
> 
> 也就是说，如果不指定ORDER BY，则分区行无序，不隐式处理顺序，并且所有分区行都是对等的。
> 来自：https://dev.mysql.com/doc/refman/8.0/en/window-functions-usage.html

> In the absence of a frame clause, the default frame depends on whether an ORDER BY clause is present:
> 
> - With ORDER BY: The default frame includes rows from the partition start through the current row, including all peers of the current row (rows equal to the current row according to the ORDER BY clause). The default is equivalent to this frame specification:
> RANGE BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
><br/>
> - Without ORDER BY: The default frame includes all partition rows (because, without ORDER BY, all partition rows are peers). The default is equivalent to this frame specification:
> RANGE BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING
>
> 来自：https://dev.mysql.com/doc/refman/8.3/en/window-functions-frames.html

也就是说，如果没有指定frame子句，默认的frame取决于是否显式指定Order by子句：
- 如果指定了ORDER BY : 默认frame从分区开始到当前行，包括和当前行对等大所有行(根据ORDER BY子句确定哪些行和当前行相等)，默认Frame相当于
```sql
RANGE BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
```
- 如果没有指定ORDER BY子句，默认frame包含分区所有行(由于没有指定OERDER BY，所有分区行对等)，默认frame相当于：
```sql
RANGE BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING
```

---

`window_function`可能是：
1. 排名函数，不可以指定frame(窗口范围)，MySql要求必须指定Order by。

	- `RANK`函数：计算排序时，如果存在相同位次的记录，则会跳过之后的位次。例如，有 3 条记录排在第 1 位时：1 位、1 位、1 位、4 位……
	
	- `DENSE_RANK`函数：同样是计算排序，即使存在相同位次的记录，也不会跳过之后的位次。例如，有 3 条记录排在第 1 位时：1 位、1 位、1 位、2 位……
	
	- `ROW_NUMBER`函数：赋予唯一的连续位次。例如，有 3 条记录排在第 1 位时：1 位、2 位、3 位、4 位……
	
	- `CUME_DIST()`
	
	- `NTILE(n)`：用于将分组数据按照顺序切分成n片，返回当前切片值。将一个有序的数据集划分为多个桶(bucket)，并为每行分配一个适当的桶数(切片值)。它可用于将数据划分为相等的小切片，为每一行分配该小切片的数字序号。使用场景，例如：统计一个cookie，pv数最多的前1/3的天
	
	    ```Sql
	    SELECT id, date, pv,
	    	NTILE(3) OVER(PARTITION BY id ORDER BY pv DESC) AS rn 
	    FROM data;
	    ```
	
	- `PERCENT_RANK()`,
	
2. 聚集函数，可以指定窗口范围(Frame)。
	- `AVG()`, `COUNT()`, `SUM()`, `MAX()`, `MIN()`
	
	```sql
	-- 一行一行累加计算,称为累计统计法
	SELECT product_id, product_name, sale_price,
		SUM(sale_price) OVER (ORDER BY product_id) AS current_avg
		FROM Product;
	
	-- 对于聚集计算可以指定Frame(窗口范围),称为移动平均(moving average)
	-- 这里ROWS(行), PRECEDING(之前), FOLLOWING(之后)
	-- 表示采用最靠近的3行(当前行,之前1行记录,之前2行记录)做汇总对象
	SELECT product_id, product_name, sale_price,
		AVG (sale_price) OVER (ORDER BY product_id ROWS 2 PRECEDING) AS moving_avg
		FROM Product;
	
	-- 表示采用前后行(当前行,之前1行记录,之后1行记录)做汇总对象
	SELECT product_id, product_name, sale_price,
		AVG(sale_price) OVER (ORDER BY product_id
			ROWS BETWEEN 1 PRECEDING AND 1 FOLLOWING) AS moving_avg
		FROM Product;
	```

3. 取值窗口函数。
	
	- `FIRST_VALUE()`,  `LAST_VALUE()`分别用于当前窗口Frame的第一行/最后一行值。
		```sql
		FIRST_VALUE(expr) [null_treatment]
		-- expr:		
		```

		> The results of the FIRST_VALUE function depends on the ordering of the data. The results are nondeterministic in the following cases:
		> - When no ORDER BY clause is specified and a partition contains two different values for an expression
		> - When the expression evaluates to different values that correspond to the same value in the ORDER BY list.
		> 来自：https://docs.aws.amazon.com/redshift/latest/dg/r_WF_first_value.html

		> 也就是说，FIRST_VALUE函数的结果取决于数据的顺序。在以下情况下，结果是不确定的:
		> - 未指定ORDER BY且一个分区有表达式有两个不同的值。
		> - ORDER BY列表中值相同，表达式计算值不同。例如，下面是一个分区的数据，ORDER BY的列是一致的，按照ORDER BY排序，expr的输出顺序可能是`1, 2, 3`, 或`2, 1, 3`,或`3, 1, 2`，那么`FIRST_VALUE(expr)`的输出结果不固定。
		>
		> | expr | ORDER BY表达式值 |
		> | :--: | :--------------: |
		> |  1   |        1         |
		> |  2   |        1         |
		> |  3   |        1         |



- `LAG()`和`LEAD()`分别用于计算用于统计窗口内向上第n行值和窗口内向下第n行值。

  ```sql
  	LAG | LEAD(col_name, offset_opt, default_opt)
  	-- offset_opt:			偏移的offset
  	-- default_opt:			超出记录窗口时的默认值
  ```

# 参考资料

1. [知乎:SQL 窗口函数的优化和执行](https://zhuanlan.zhihu.com/p/80051518)
2. [Drill:SQL Window Functions Introduction](https://drill.apache.org/docs/sql-window-functions-introduction/)
3. [MySQL:Window Function Concepts and Syntax](https://dev.mysql.com/doc/refman/8.0/en/window-functions-usage.html)
