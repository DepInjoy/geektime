
SQL字符串下发给Postgres到查询优化，调用栈：
```c
exec_simple_query(query_string)
    // 1. 语法解析
    pg_parse_query
        raw_parser
    // 2. Analyze和rewrite
    pg_analyze_and_rewrite_fixedparams
        // Analyse原始语法树并转化为Query
        parse_analyze_fixedparams
        // Rewrite the query
        pg_rewrite_query
    // 3. 执行查询优化
    pg_plan_queries
        pg_plan_query
            planner     // 查询优化入口
```
# Parser
`src/backend/parser/gram.y`词法分析，`src/backend/parser/scan.l`语法分析。
```C
// 将用户输入字符串形式的SQL解析成查询树
List * raw_parser(const char *str, RawParseMode mode)
```
## 语法树数据结构

PostgreSQL中的结构体采用了统一的形式，它们都是基于`Node`结构体进行的“扩展”，Node结构体中只包含一个`NodeTag`成员变量，`NodeTag`是枚举类型
```C
// src/include/nodes/nodes.h
typedef struct Node {
	NodeTag		type;
} Node;
```
以常见的简单的SELECT语句为例，了解其查询树的数据结构，在语法分析中可以看到
```
simple_select:
    SELECT opt_all_clause opt_target_list
    into_clause from_clause where_clause
    group_clause having_clause window_clause
        {
            SelectStmt *n = makeNode(SelectStmt);
            n->targetList = $3;
            n->intoClause = $4;
            n->fromClause = $5;
            n->whereClause = $6;
            n->groupClause = ($7)->list;
            n->groupDistinct = ($7)->distinct;
            n->havingClause = $8;
            n->windowClause = $9;
            $$ = (Node *) n;
        }
```

makeNode接口用于创建Node结构体，并给type赋值(T_SelectStmt)
```C
// src/include/nodes/nodes.h
#define newNode(size, tag) \
({	Node   *_result; \
	_result = (Node *) palloc0fast(size); \
	_result->type = (tag); \
	_result; \
})

#define makeNode(_type_)		((_type_ *) newNode(sizeof(_type_),T_##_type_))
```
可见，SELECT语句的查询树采用`SelectStmt`结构表达
```C++
typedef struct SelectStmt {
    // makeNode接口给type赋值,即T_SelectStmt
	NodeTag		type;

	List	   *distinctClause;
	IntoClause *intoClause;		/* target for SELECT INTO */
	List	   *targetList;		/* the target list (of ResTarget) */
	List	   *fromClause;		/* the FROM clause */
	Node	   *whereClause;	/* WHERE qualification */
	List	   *groupClause;	/* GROUP BY clauses */
	bool		groupDistinct;	/* Is this GROUP BY DISTINCT? */
	Node	   *havingClause;	/* HAVING conditional-expression */
	List	   *windowClause;	/* WINDOW window_name AS (...), ... */

	List	   *valuesLists;	/* untransformed list of expression lists */

	List	   *sortClause;		/* sort clause (a list of SortBy's) */
	Node	   *limitOffset;	/* # of result tuples to skip */
	Node	   *limitCount;		/* # of result tuples to return */
	LimitOption limitOption;	/* limit type */
	List	   *lockingClause;	/* FOR UPDATE (list of LockingClause's) */
	WithClause *withClause;		/* WITH clause */

	SetOperation op;			/* type of set op */
	bool		all;			/* ALL specified? */
	struct SelectStmt *larg;	/* left child */
	struct SelectStmt *rarg;	/* right child */
} SelectStmt;
```
```c
// src/include/nodes/primnodes.h
// 语法树FROM clause, relation_expr经list_make1处理为List
typedef struct RangeVar {
	NodeTag		type;
	char	   *catalogname;
	char	   *schemaname;
	char	   *relname;
	bool		inh;
	char		relpersistence;
	Alias	   *alias;
	int			location;
} RangeVar;
```
`List`也是对Node的扩展，它的第一个成员也是`NodeTag`用于表示不同类型的List
```c
// src/include/nodes/pg_list.h
typedef struct List {
    // 表示不同类型的List
	NodeTag		type;
    // 当前elements的数量
	int			length;
    //  allocated length of elements[]
	int			max_length;
    // 
	ListCell   *elements;
	ListCell	initial_elements[FLEXIBLE_ARRAY_MEMBER];
} List;
```

# Analyze
`Query *parse_analyze_fixedparams(RawStmt *parseTree, ...)`实现对原始的语法树Analyze并转化为`Query`。首先了解一下`Query`结构。

## Query数据结构
```C
// src/include/nodes/parsenodes.h
typedef struct Query {
    // 取值为T_Query
	NodeTag		type;
	CmdType		commandType;
	QuerySource querySource pg_node_attr(query_jumble_ignore);
            ......
} Query;
```
### Var结构
```C
typedef struct Var {
	Expr		xpr;
    // 列属性所在表在Query->rtable中的rtindex
	int			varno;
    // 列属性在表中的编号(第几列)
	AttrNumber	varattno;

	// 列属性对应的类型
	Oid			vartype pg_node_attr(query_jumble_ignore);
	// 列属性的精度(长度)
	int32		vartypmod pg_node_attr(query_jumble_ignore);
	/* OID of collation, or InvalidOid if none */
	Oid			varcollid pg_node_attr(query_jumble_ignore);
	Bitmapset  *varnullingrels pg_node_attr(query_jumble_ignore);

	// 列属性的相对位置,和子查询相关
    // 确定列属性对应的表所在的层次(相对值)
	Index		varlevelsup;

	Index		varnosyn pg_node_attr(equal_ignore, query_jumble_ignore);
	AttrNumber	varattnosyn pg_node_attr(equal_ignore, query_jumble_ignore);

	// 列属性出现在SQL语句中的位置
	int			location;
} Var;
```

### RangeTblEntry结构
RangeTblEntry(范围表，简称RTE)描述了查询中出现的表，它通常出现在查询语句的FROM子句中，范围表中既有常规意义上的堆表，还有子查询、连接表等。
```c
typedef struct RangeTblEntry{
	NodeTag		type;
    // 范围表的类型
	RTEKind		rtekind;

    // For RTE_RELATION,普通的表
	Oid			relid;
	char		relkind;
	int			rellockmode;
	struct TableSampleClause *tablesample;
	Index		perminfoindex;

	// for RTE_SUBQUERY,子查询类型的表
	Query	   *subquery;
	bool		security_barrier;

    // For RTE_JOIN,连接类型的表
	JoinType	jointype;
	int			joinmergedcols;
    // Join的表的所有列集合
	List	   *joinaliasvars;
	List	   *joinleftcols;
	List	   *joinrightcols;

                ......
} RangeTblEntry;
```
### RangeTblRef结构
`RangeTblEntry`只保留在查询树的`Query->rtable`链表中，而链表是一个线性结构，它如何保存树状的关系代数表达式中的连接操作呢？答案是在`Query->jointree`中保存各个范围表之间的连接关系。

如果在`Query->jointree`中还保存同样的一份`RangeTblEntry`，那么一方面会造成存储的冗余，另一方面也容易产生数据不一致的问题，因此在查询树的其他任何地方都不再存放新的`RangeTblEntry`，每个范围表在Query->rtable链表中有且只能有一个，在其他地方使用到范围表都使用`RangeTblRef`来代替。`RangeTblRef`是对`RangeTblEntry`的引用，因为`RangeTblEntry`在`Query->rtable`中的位置是确定的，因此可以用它在`Query->rtable`链表中的位置`rtindex`来标识。

```C
// src/include/nodes/primnodes.h
typedef struct RangeTblRef {
	NodeTag		type;
	int			rtindex;
} RangeTblRef;
```
### 表连接
查询语句中如果显式地指定两个表之间的连接关系，例如`A LEFT JOIN B ON Pab`这种形式，用`JoinExpr`来表示
```c
typedef struct JoinExpr {
	NodeTag		type;
    // Join类型，例如JOIN_INNER等
	JoinType	jointype;
    // 是否是自然连接
	bool		isNatural;
    // 连接操作的LHS(左侧)表
	Node	   *larg;
    // 连接操作的RHS(右侧)表
	Node	   *rarg;
    // USING子句对应的约束条件
	List	   *usingClause pg_node_attr(query_jumble_ignore);
	/* alias attached to USING clause, if any */
	Alias	   *join_using_alias pg_node_attr(query_jumble_ignore);
	// on子句对应的约束条件
	Node	   *quals;
	// 连接操作的投影列
	Alias	   *alias pg_node_attr(query_jumble_ignore);
	// 当前JoinExpr对应的RangeTblRef->rtindex
	int			rtindex;
} JoinExpr;
```

`FromExpr`和`JoinExpr`是用来表示表之间的连接关系的结构体。通常来说，`FromExpr`中的各个表之间的连接关系是Inner Join，这样就可以在`FromExpr->fromlist`中保存任意多个表，默认是内连接的关系
```C
typedef struct FromExpr {
	NodeTag		type;
    // FromExpr中包含的表
	List	   *fromlist;
    // fromlist中表间的约束条件
	Node	   *quals;
} FromExpr;
```



# 查询优化

查询优化的入口在`src\backend\optimizer\plan\planner.c`的`planner`接口,它支持通过`planner_hook`来自定义优化方法，如果没有采用PG自定义的标准的查询优化函数`standard_planner`。
```C++
PlannedStmt * planner(Query *parse, const char *query_string,
                      int cursorOptions, ParamListInfo boundParams)
```

