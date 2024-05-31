
查询优化的入口在`src\backend\optimizer\plan\planner.c`的`planner`接口，其调用栈：
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
## 查询树数据结构

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

# 查询优化

它支持通过`planner_hook`来自定义优化方法，如果没有采用PG自定义的标准的查询优化函数`standard_planner`。
```C++
PlannedStmt * planner(Query *parse, const char *query_string,
                      int cursorOptions, ParamListInfo boundParams)
```

