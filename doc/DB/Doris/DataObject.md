离线场景下，BITMAP类型数据导入速度慢，在数据量大的情况下查询速度慢于HLL类型数据的查询速度，但优于COUNTDISTINCT。注意：实时场景下，BITMAP如果不使用全局字典，使用BITMAP_HASH（）函数计算去重值可能导致千分之一左右的误差。

HLL类型源自HLL算法。HLL（英文全称HyperLogLog）算法是统计数据集中唯一值个数的高效近似算法，有着计算速度快、节省空间的特点，不需要直接存储集合本身，而是存储一种名为HLL的数据结构。HLL算法在计算速度和存储空间上都有优势。在时间复杂度上，Sort算法排序时长至少为$O(nlogn)。虽然Hash算法和HLL算法一样只需扫描一次全表，仅需$O(n)$的时间就可以得出结果，但是存储空间上，Sort算法和Hash算法都需要先把原始数据存储起来再进行统计，会导致存储空间消耗巨大，而HLL算法不需要存储原始数据，只需要维护HLL数据结构，故占用空间始终是1280B。
HLL类型的用法和BITMAP类型类似，仅用于Aggregate数据模型的表，且不能用在Key列，建表时需搭配HLL_UNION使用。用户不需要指定HLL类型数据长度和默认值。长度根据数据的聚合程度调整。HLL类型数据只能通过配套的`HLL_UNION_AGG`、`HLL_RAW_AGG`、`HLL_CARDINALITY`、`HLL_HASH`进行查询或调用。

在数据量大的情况下，HLL类型模糊去重性能优于COUNT DISTINCT和BITMAP_UNION_COUNT。HLL算法的误差率通常在1%左右，默认可计算去重数据的最大数量为1.6e12，误差率最大可达2.3%。注意：如果去重结果超过默认规格会导致计算结果误差变大，或计算失败并报错。

# 表定义

```sql
CREATE TABLE [IF NOT EXISTS] [database.]table
(
    column_definition_list
    [, index_definition_list]
)
[engine_type]
[keys_type]
[table_comment]
[partition_info]
distribution_desc
[rollup_list]
[properties]
[extra_properties]

column_definition_list : column_definition[, column_definition]
column_definition   :  column_name column_type [KEY] [aggr_type] [NULL] [AUTO_INCREMENT] [default_value] [column_comment]
```

ENGINE默认支持表类型为OLAP，还支持MySQL、Broker、Hive、Iceberg四种外部表。

## 列定义

列定义即确定表的每一列对应的属性，包括列名、列类型、可选的聚合类型、是否为空和默认值。
```sql
column_definition :
    column_name column_type [KEY] [aggr_type]
    [NULL] [AUTO_INCREMENT] [default_value] [column_comment]
```

aggr_type仅用于Aggregate模型，表示聚合类型，支持以下聚合类型：
```    
SUM：求和。适用数值类型。
MIN：求最小值。适合数值类型。
MAX：求最大值。适合数值类型。
REPLACE：替换。对于维度列相同的行，指标列会按照导入的先后顺序，后导入的替换先导入的。
REPLACE_IF_NOT_NULL：非空值替换。和 REPLACE 的区别在于对于null值，不做替换。这里要注意的是字段默认值要给NULL，而不能是空字符串，如果是空字符串，会给你替换成空字符串。
HLL_UNION：HLL 类型的列的聚合方式，通过 HyperLogLog 算法聚合。
BITMAP_UNION：BIMTAP 类型的列的聚合方式，进行位图的并集聚合。
```
它是可选项，如果不指定，说明该列是维度列(Key列)，否则是事实列(Value列)。在建表语句中，所有的Key列必须在Value列之前，一张表可以没有Value列(这样的表叫作维度表)，但不能没有Key列。默认情况下，所有的列允许值为NULL(导入时用`\N`来表示)。

## 键描述

键描述是指对表的数据模型及主键进行明确定义，键描述内容包括键类型和键清单。
```sql
key_type(col1, col2, ...)
```
Doris键类型有Aggregate Key、Unique Key、Duplicate Key。StarRocks在Doris键类型Unique Key的基础上增加了Primary Key，用Delete+Insert策略替换Unique Key的Merge策略，实现了更高效的查询性能。键清单用来确定Doris的Key列排序，且不同的键类型有不同的键清单要求，具体如下。
- Aggregate Key：Key列相同的记录，Value列按照指定的聚合类型聚合，要求所有未定义聚合类型的列都写入键清单。
- Unique Key：Key列相同的记录，Value列按导入顺序进行覆盖，键清单为该表的主键字段（支持联合主键）。
- Duplicate Key：Key列相同的记录，键清单一般为排序字段或者查询字段，主要用于索引优化。在不指定的情况下，Doris表默认为Duplicate Key类型，Key列为列定义中前36B，如果前36B的列数小于3，将使用前三列。

## 分布描述
```sql
-- Hash分桶语法
DISTRIBUTED BY HASH (k1[,k2 ...]) [BUCKETS num|auto]

-- Random分桶语法
DISTRIBUTED BY RANDOM [BUCKETS num|auto]
```

## 键值对
针对OLAP类型的表引擎，PROPERTIES主要用于设置表的存储特性、索引特性、动态分区特性、内存表特性等。
```

```
# 参考资料
1. Doris实时数仓实战
2. [CREATE-TABLE](https://doris.apache.org/zh-CN/docs/dev/sql-manual/sql-reference/Data-Definition-Statements/Create/CREATE-TABLE/)