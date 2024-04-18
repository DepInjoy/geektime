窗口计算在投影(上图SELECT选择列)-> 窗口WINDOW -> DISTINCT去重。

窗口算子在prepare阶段拆分情况
```sql
-- 情况1: 窗口+agg拆分
select sum(sum(a)) over(PARTITION by a order by b)
	from tbl
	GROUP by a;

-- 拆分为
select sum(im2) over(PARTITION by a order by b)
	from (
		select a, b, sum(a) as im2 from tbl
			GROUP by a
    );

-- 情况2:窗口2个及以上分区条件拆分
SELECT
	-- level 1 window
	sum(a) over (PARTITION by a),
	count(a) over(PARTITION by a),
	-- level 2 window
	rank() over (PARTITION by c order by c)
	from tbl;

-- 拆分为
SELECT
	rank() over (PARTITION by c order by c)
	from (
		select c,
			sum(a) over (PARTITION by a),
			count(a) over(PARTITION by a)
		from tbl;
	);

-- 情况3:窗口函数出现在orderby
select a
    from tbl
    order by sum(a) over(PARTITION by a order by a);

-- 拆分为
select a from (
    select a, sum(a) over(PARTITION by a order by a) win
    from tbl
) order by win;


-- 情况4:orderby窗口+Agg
SELECT DISTINCT tbl.STATUS_ID
	FROM tbl
	ORDER BY ROW_NUMBER() OVER (PARTITION BY a ORDER BY a);

-- 拆分为
SELECT dis_status_id from(
    SELECT dis_status_id, ROW_NUMBER() OVER (PARTITION BY a ORDER BY a) win 
        from (
	        SELECT DISTINCT UD.STATUS_ID as dis_status_id
		        FROM tbl UD
        )
) ORDER by win;
```