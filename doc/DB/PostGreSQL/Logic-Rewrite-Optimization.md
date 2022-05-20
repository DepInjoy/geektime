查询优化实现相关代码路径`backend\optimizer\plan\planner.c`

```c
/**
 * @brief:	查询优化模块的入口函数，它会检查当前库是否定义了自定义的优化方法planner_hook
 * 			如果planner_hook为NULL，则调用standard_planner函数
 * 			standard_planner函数是PG查询优化的标准函数
*/
PlannedStmt *
planner(Query *parse, const char *query_string, int cursorOptions,
		ParamListInfo boundParams)
{
	PlannedStmt *result;

	if (planner_hook) // 自定义查询优化
		result = (*planner_hook) (parse, query_string, cursorOptions, boundParams);
	else // PG查询优化标准函数
		result = standard_planner(parse, query_string, cursorOptions, boundParams);
	return result;
}
```

接下来，主要是理解PG查询优化标准函数的实现逻辑

```c
/**
 * @brief PG查询优化标准函数
 */
PlannedStmt *
standard_planner(Query *parse, const char *query_string, int cursorOptions,
				 ParamListInfo boundParams)
```

逻辑优化被划可以划分为逻辑重写优化和逻辑分解优化。划分依据是：逻辑重写优化阶段主要还是对查询树重写，即在查询树上进行改造，改造后还是一颗查询树，在逻辑分解阶段，会将查询树打散，重新建立等价于查询树的逻辑关系。

# 逻辑重写优化

## 通用表达式

通用表达式对应的是WITH语句，作用和子查询类似，是一个单独的逻辑操作，DB不对通用表达式做提升优化。因为通用表达式实现的字表常会被多次使用，具有“一次求值，多次使用”的特点。

```
/*
 * SQL standard WITH clause looks like:
 *
 * WITH [ RECURSIVE ] <query name> [ (<column>,...) ]
 *		AS (query) [ SEARCH or CYCLE clause ]
 *
 * Recognizing WITH_LA here allows a CTE to be named TIME or ORDINALITY.
 */
with_clause:
```

其中，语法树表示，相关实现位于`src\include\nodes\parsenodes.h`

```c
typedef struct WithClause
{
	NodeTag		type;
	List	   *ctes;			/* list of CommonTableExprs */
	bool		recursive;		/* true = WITH RECURSIVE */
	int			location;		/* token location, or -1 if unknown */
} WithClause;
```



## 子查询提升



### 提升子连接

PostgreSQL数据库依据不同谓词区分SUBLINK的类型。

```c
typedef enum SubLinkType
{
	EXISTS_SUBLINK, 	// [NOT] EXISTS谓词
	ALL_SUBLINK,		// ALL谓词
	ANY_SUBLINK,		// ANY/IN/SOME谓词
	ROWCOMPARE_SUBLINK,
	EXPR_SUBLINK,
	ARRAY_SUBLINK,
	CTE_SUBLINK,				/* for SubPlans only */
	INITPLAN_FUNC_SUBLINK,		/* for function run as initplan */
	NOT_EXISTS_SUBLINK /* GPORCA uses NOT_EXIST_SUBLINK to implement correlated left anti semijoin. */
} SubLinkType;
```

PostgreSQL主要针对`ANY_SUBLIN`K和`EXISTS_SUBLINK`两种类型的子连接尝试进行提升，通过`backend\parser\gram.y`文件发现主要是`IN/ANY`谓词生成`ANY_SUBLINK`，EXISTS谓词生成`EXISTS_SUBLINK`。

| 谓词         | 形式              | 描述                                           |      |
| ------------ | ----------------- | ---------------------------------------------- | ---- |
| [NOT] IN     | LH [NOT] in EXPR  | 如果提升，则变成[反]半连接，即[Anti-]Semi Join |      |
| ANY/SOME     | LH OP ANY EXPR    | 如果提升，则变成半连接，即Semi Join            |      |
| [NOT] EXISTS | [NOT] EXISTS EXPR | 如果提升，则变成[反]半连接，即[Anti-]Semi Join |      |


PostgreSQL为子连接定义的数据结构，相关定义位于`src\include\nodes\primnodes.h`
```c
typedef struct SubLink
```





```sql
explain
select * from student where exists (select sno from score where score.sno=student.sno);
```

```text
"Hash Join  (cost=26.25..52.62 rows=450 width=62)"
"  Hash Cond: (student.sno = score.sno)"
"  ->  Seq Scan on student  (cost=0.00..19.00 rows=900 width=62)"
"  ->  Hash  (cost=23.75..23.75 rows=200 width=4)"
"        ->  HashAggregate  (cost=21.75..23.75 rows=200 width=4)"
"              Group Key: score.sno"
"              ->  Seq Scan on score  (cost=0.00..19.40 rows=940 width=4)"
```





```sql
explain
select * from student where exists (select sno from score);
```

```
"Result  (cost=0.02..19.02 rows=900 width=62)"
"  One-Time Filter: $0"
"  InitPlan 1 (returns $0)"
"    ->  Seq Scan on score  (cost=0.00..19.40 rows=940 width=0)"
"  ->  Seq Scan on student  (cost=0.02..19.02 rows=900 width=62)"
```



### 提升子查询

## UNION ALL优化

## 展开继承表

## 预处理表达式

##  处理HAVING子句

## GROUP BY键值消除

## 外连接消除





## 逻辑分解优化

