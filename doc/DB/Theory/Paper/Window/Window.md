窗口函数也称为OLAP函数，其中OLAP是Online Analytical Processing的简称，意思是对数据库数据实时分析处理。例如，市场分析、创建财务报表、创建计划等日常性商务工作。

`PARTITION BY`在横向上对表进行分组，`ORDER BY`决定了纵向排序的规则，通过`PARTITION BY`分组后的记录集合称为窗口(代表范围)。如果不指定`PARTITION BY`则表示将整个表作为一个大的窗口来计算。

排名函数，不可以指定窗口范围。

- `RANK`函数：计算排序时，如果存在相同位次的记录，则会跳过之后的位次。例）有 3 条记录排在第 1 位时：1 位、1 位、1 位、4 位……
- `DENSE_RANK`函数：同样是计算排序，即使存在相同位次的记录，也不会跳过之后的位次。例）有 3 条记录排在第 1 位时：1 位、1 位、1 位、2 位……
- `ROW_NUMBER`函数：赋予唯一的连续位次。例）有 3 条记录排在第 1 位时：1 位、2 位、3 位、4 位……

聚集函数，可以指定窗口范围(Frame)。

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

