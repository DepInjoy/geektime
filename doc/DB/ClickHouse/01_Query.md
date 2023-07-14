# WITH子句

ClickHouse支持CTE（Common Table Expression，公共表表达式），以增强查询语句的表达。例如

```sql
SELECT pow(pow(2, 2), 3)
```

改用CTE的形式后，可以极大地提高语句的可读性和可维护性

```sql
WITH pow(2, 2) AS a SELECT pow(a, 3)
```



CTE通过WITH子句表示，支持下面四种用法

1. 定义变量，定义的变量可以在后续的查询子句中被直接访问

    ```sql
    WITH 10 AS start
    	SELECT number FROM system.numbers WHERE number > start LIMIT 5
    ```

2. 调用函数，可以访问SELECT子句中的列字段，并调用函数做进一步的加工处理。

    ```sql
    -- 对data_uncompressed_bytes使用聚合函数求和后
    -- 之后, 在SELECT子句中对其进行格式化处理
    WITH SUM(data_uncompressed_bytes) AS bytes
    	SELECT database , formatReadableSize(bytes) AS format 			FROM system.columns
    		GROUP BY database
    		ORDER BY bytes DESC
    ```

3. 定义子查询

    ```sql
    WITH (
        	-- 计算数据总和
        	SELECT SUM(data_uncompressed_bytes) FROM system.columns) AS total_bytes
        SELECT database,
        		-- 计算为压缩数据大小和数据总和的比例
        		(SUM(data_uncompressed_bytes) / total_bytes) * 100 AS database_disk_usage
        	FROM system.columns 
        	GROUP BY database
        	ORDER BY database_disk_usage DES
    ```

    

4. 子查询中重复使用WITH，子查询中可以嵌套WITH子句

    ```sql
    WITH (round(database_disk_usage)) AS database_disk_usage_v1
        SELECT database, database_disk_usage, database_disk_usage_v1
        	FROM ( -- 子查询嵌套WITH
                WITH (SELECT SUM(data_uncompressed_bytes) FROM system.columns) AS total_bytes
                SELECT database , 
                		(SUM(data_uncompressed_bytes) / total_bytes) * 100 AS database_disk_usage
                	FROM system.columns
                	GROUP BY database
                	ORDER BY database_disk_usage DESC
            )
    ```

    # FROM子句

    FROM子句表示从何处读取数据，目前支持如下3种形式。

    1. 从数据表中取数

        ```sql
        SELECT WatchID FROM hits_v1
        ```

    2. 从子查询中取数：

        ```sql
        SELECT MAX_WatchID
        	FROM (
                SELECT MAX(WatchID) AS MAX_WatchID FROM hits_v1);
        ```

    3. 从表函数中取数：

        ```sql
        SELECT number FROM numbers(5);
        ```

    ClickHouse中没有`DUAL`虚拟表，采用`system.one` 

    ```sql
    SELECT 1
    -- 等价
    SELECT 1 FROM system.one;
    ```

    在FROM子句后，可以使用Final修饰符。它可以配合CollapsingMergeTree和Versioned-CollapsingMergeTree等表引擎进行查询操作，以强制在查询过程中合并，但由于Final修饰符会降低查询性能，所以应该尽可能避免使用它。

# SAMPLE子句

SAMPLE子句能够实现数据采样的功能，使查询仅返回采样数据而不是全部数据，从而有效减少查询负载。SAMPLE子句的采样机制是一种幂等设计，也就是说在数据不发生变化的情况下，使用相同的采样规则总是能够返回相同的数据，所以这项特性非常适合在那些可以接受近似查询结果的场合使用。例如在数据量十分巨大的情况下，对查询时效性的要求大于准确性时就可以尝试使用SAMPLE子句。

SAMPLE子句只能用于MergeTree系列引擎的数据表，并且要求在`CREATE TABLE`时声明`SAMPLE BY`抽样表达式

```sql
CREATE TABLE hits_v1 (
	CounterID UInt64,
    EventDate DATE,
    UserID UInt64
) ENGINE = MergeTree()
	PARTITION BY toYYYYMM(EventDate)
	ORDER BY (CounterID, intHash32(UserID))
	--Sample Key声明的表达式必须也包含在主键的声明中
	SAMPLE BY intHash32(UserID);
```

SAMPLE子句目前支持如下3种用法

- `SAMPLE factor`表示按因子系数采样，其中`factor`表示采样因子，它的取值支持0～1之间的小数。如果factor设置为0或者1，则效果等同于不进行数据采样。

    ```sql
    -- 按10%的因子采样数据
    SELECT CounterID FROM hits_v1 SAMPLE 0.1;
    SELECT CounterID FROM hits_v1 SAMPLE 1/10;
    ```

- `SAMPLE rows`表示按样本数量采样，其中`rows`表示至少采样多少行数据，它的取值必须是大于1的整数。如果rows的取值大于表内数据的总行数，则效果等于`rows=1`（即不使用采样)。采样数据的最小粒度由`index_granularity`索引粒度决定。

    ```sql
    -- 采样10000行数据
    SELECT count() FROM hits_v1 SAMPLE 10000;
    ```

    可以使用虚拟字段_sample_factor来获取当前查询所对应的采样系数

    ```sql
    SELECT CounterID,_sample_factor FROM hits_v1 SAMPLE 100000 LIMIT 1;
    ```

- `SAMPLE factor OFFSET n`表示按因子系数和偏移量采样，其中factor表示采样因子，n表示偏移多少数据后才开始采样，它们两个的取值都是0～1之间的小数。如果在计算OFFSET偏移量后，按照SAMPLE比例采样出现了溢出，则数据会被自动截断。

    ```sql
    -- 从数据的二分之一处开始，按0.4的系数采样数据
    SELECT CounterID FROM hits_v1 SAMPLE 0.4 OFFSET 0.5;
    ```

    这种用法支持使用十进制的表达形式，也支持虚拟字段`_sample_factor`：

    ```sql
    SELECT CounterID,_sample_factor FROM hits_v1 SAMPLE 1/10 OFFSET 1/2;
    ```

# ARRAY JOIN子句

ARRAY JOIN子句允许在数据表的内部，与数组或嵌套类型的字段进行JOIN操作，从而将一行数组展开为多行。支持INNER和LEFT两种JOIN策略，默认采用INNER JOIN策略。

```sql
CREATE TABLE query_v1(
	title String,
    value Array(UInt8)
) ENGINE = Log;

INSERT INTO query_v1 VALUES ('food', [1,2,3]), ('fruit', [3,4]), ('meat', []);
SELECT title,value,v FROM query_v1 ARRAY JOIN value AS v;
SELECT title,value,v FROM query_v1 LEFT ARRAY JOIN value AS v;
```

当同时对多个数组字段进行ARRAY JOIN操作时，查询的计算逻辑是按行合并而不是产生笛卡儿积

```sql
-- ARRAY JOIN多个数组时, 是合并, 不是笛卡儿积
SELECT title, value, v, arrayMap(x -> x * 2, value) as mapv, v_1
	FROM query_v1 LEFT ARRAY JOIN value AS v , mapv as v_1
```



嵌套数据类型的本质是数组，所以ARRAY JOIN也支持嵌套数据类型。

```sql
--ARRAY JOIN嵌套类型
CREATE TABLE query_v2(
    title String,
    nest Nested(
        v1 UInt32,
        v2 UInt64)
) ENGINE = Log;

-- 写测试数据
-- 同一行数据，数组长度要对齐
-- 多行数据之间的数组长度没有限制
INSERT INTO query_v2 VALUES
	('food', [1,2,3], [10,20,30]), 
	('fruit', [4,5], [40,50]), 
	('meat', [], []);

-- 直接使用字段别名
SELECT title, nest.v1, nest.v2
	FROM query_v2 ARRAY JOIN nest;

-- 使用点访问符
SELECT title, nest.v1, nest.v2
	FROM query_v2 ARRAY JOIN nest.v1, nest.v2;

-- 也可以只ARRAY JOIN其中部分字段
-- 只有被ARRAY JOIN的数组才会展开
SELECT title, nest.v1,
 nest.v2 FROM query_v2 ARRAY JOIN nest.v1;
```

# Join子句

JOIN子句可以对左右两张表的数据进行连接，这是最常用的查询子句之一。它的语法包含连接精度和连接类型两部分，外连接失配字段采用列字段的默认值补全，连接查询的空值策略是通过`join_use_nulls`参数指定的，默认为0。当参数值为0时，空值由数据类型的默认值填充；而当参数值为1时，空值由`NULL`填充。

```
					LEFT/RIGTH/FULL[outer]
						  (外连接)
						  
ALL/ANY/ASOF	+ 			INNER				+ 		JOIN
 (连接精度)					(内连接)
 						
 						   CROSS
 						 (交叉连接)
```

连接精度决定了JOIN查询在连接数据时所使用的策略，目前支持ALL、ANY和ASOF三种类型。如果不主动声明，则默认是ALL。可以通过`join_default_strictness`配置参数修改默认的连接精度类型。对数据是否连接匹配的判断是通过JOIN KEY进行的，目前只支持等值连接。CROSS JOIN不需要使用JOIN KEY，产生笛卡儿积。



ALL：如果左表内的一行数据，在右表中有多行数据与之连接匹配，则返回右表中全部连接的数据。而判断连接匹配的依据是左表与右表内的数据，基于连接键(JOIN KEY)的取值完全相等(equal)。

```sql
SELECT a.id,a.name,b.rate FROM join_tb1 AS a
	ALL INNER JOIN join_tb2 AS b ON a.id = b.id;
```



ANY：如果左表内的一行数据，在右表中有多行数据与之连接匹配，则仅返回右表中第一行连接的数据。ANY与ALL判断连接匹配的依据相同。

```sql
SELECT a.id,a.name,b.rate FROM join_tb1 AS a
	ANY INNER JOIN join_tb2 AS b ON a.id = b.id
```

ASOF：模糊连接，它允许在连接键之后追加定义一个模糊连接的匹配条件`asof_column`。`asof_colum`必须是整型、浮点型和日期型这类有序序列的数据类型；asof_colum不能是数据表内的唯一字段，也就是说，JOIN KEY和`asof_colum`不能是同一字段。

```sql
-- a.id = b.id连接条件
-- a.time = b.time模糊连接条件
-- 等价于a.id = b.id AND a.time >= b.time
SELECT a.id,a.name,b.rate,a.time,b.time
	FROM join_tb1 AS a ASOF INNER JOIN join_tb2 AS b
	ON a.id = b.id AND a.time = b.time
```

ASOF支持使用USING的简写形式，USING后声明的最后一个字段会被自动转换成asof_colum模糊连接条件。

```sql
-- time自动转换为模糊连接条件
SELECT a.id, a.name,b.rate,a.time,b.time
	FROM join_tb1 AS a ASOFINNER JOIN join_tb2 AS b
	USING(id, time)
```



# WHERE与PREWHERE子句

PREWHERE目前只能用于MergeTree系列的表引擎，它可以看作对WHERE的一种优化，其作用与WHERE相同，均是用来过滤数据。它们的不同之处在于，使用PREWHERE时，首先只会读取PREWHERE指定的列字段数据，用于数据过滤的条件判断。待数据过滤之后再读取SELECT声明的列字段以补全其余属性。所以在一些场合下，PREWHERE相比WHERE而言，处理的数据量更少，性能更高。

为ClickHouse实现了自动优化的功能，会在条件合适的情况下将WHERE替换为PREWHERE。如果想开启这项特性，需要将`optimize_move_to_prewhere`设置为1(默认值为1，即开启状态)。在以下情形时并不会自动优化：

- 使用了常量表达式

    ```sql
    SELECT id,url,v1,v2,v3,v4 FROM query_v3 WHERE 1=1;
    ```

- 使用了默认值为ALIAS类型的字段

    ```sql
    SELECT id,url,v1,v2,v3,v4 FROM query_v3 WHERE v4 = 'A000';
    ```

- 包含了arrayJoin、globalIn、globalNotIn或者indexHint的查询

    ```sql
    SELECT title, nest.v1, nest.v2
    	FROM query_v2 ARRAY JOIN nest WHERE nest.v1=1;
    ```

- SELECT查询的列字段与WHERE谓词相同

    ```sql
    SELECT v3 FROM query_v3 WHERE v3 = 1;
    ```

- 使用了主键字段

    ```sql
    SELECT id FROM query_v3 WHERE id = 'A000';
    ```

    

# GROUP BY子句

当聚合查询内的数据存在NULL值时，ClickHouse会将NULL作为NULL=NULL的特定值处理。ClickHouse的聚合查询目前还能配合

- `WITH ROLLUP`：按照聚合键从右向左上卷数据，基于聚合函数依次生成分组小计和总计。如果设聚合键的个数为n，则最终会生成小计的个数为n+1。
- `WITH CUBE`：基于聚合键之间所有的组合生成小计信息。如果设聚合键的个数为n，则最终小计组合的个数为$2^n$。
- `WITH TOTALS`：会基于聚合函数对所有数据进行总计。

三种修饰符获取额外的汇总信息。



# HAVING子句

聚合计算之后实现二次过滤数据。

# ORDER BY子句

ORDER BY子句通过声明排序键来指定查询数据返回时的顺序。对于数据中NULL值的排序，目前ClickHouse拥有NULL值最后和NULL值优先两种策略，可以通过NULLS修饰符进行设置：

- `NULLS LAST`：NULL值排在最后(默认)。此时，数据的排列顺序为其他值`(value) -> NaN -> NULL`。
- `NULLS FIRST`：NULL值排在最前，此时，数据的排列顺序为`NULL -> NaN -> 其他值(value)`

# LIMIT BY子句
`LIMIT BY`子句和常见的`LIMIT`不同，它运行于`ORDER BY`之后和`LIMIT`之前，能够按照指定分组，最多返回前n行数据（如果数据少于n行，则按实际数量返回），常用于TOP N的查询场景。LIMIT BY的常规语法如下：

```sql
LIMIT n BY express
```

除了常规语法以外，`LIMIT BY`也支持跳过`OFFSET`偏移量获取数据，具体语法如下：

```sql
LIMIT n OFFSET y BY express
--简写
LIMIT y,n BY expres
```

 # LIMIT子句

LIMIT子句用于返回指定的前n行数据，常用于分页场景，它的三种语法形式如下所示：

```sql
LIMIT n
LIMIT n OFFSET m
LIMIT m, n
```

# SELECT子句

SELECT子句决定了一次查询语句最终返回哪些列字段或表达式。在选择列字段时，ClickHouse还为特定场景提供了一种基于正则查询的形式。

```sql
-- 返回名称以字母n开头和包含字母p的列字段
SELECT COLUMNS('^n'), COLUMNS('p') FROM system.databases
```

# DISTINCT 子句

对于NULL值而言，DISTINCT也遵循着NULL=NULL的语义，所有的NULL值都会归为一组。



# UNION ALL子句

`UNION ALL`子句能够联合左右两边的两组子查询，将结果一并返回。



ClickHouse目前并没有直接提供EXPLAIN查询，但是借助后台的服务日志，能变相实现该功能。例如，执行下面的语句，就能看到SQL的执行计划：

```shell
clickhouse-client -h ch7.nauu.com --send_logs_level=trace <<< 'SELECT * FROM hits_v1' > /dev/null
```

