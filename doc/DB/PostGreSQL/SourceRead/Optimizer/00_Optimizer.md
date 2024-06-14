
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
    NodeTag        type;
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
({    Node   *_result; \
    _result = (Node *) palloc0fast(size); \
    _result->type = (tag); \
    _result; \
})

#define makeNode(_type_)        ((_type_ *) newNode(sizeof(_type_),T_##_type_))
```
可见，SELECT语句的查询树采用`SelectStmt`结构表达
```C++
typedef struct SelectStmt {
    // makeNode接口给type赋值,即T_SelectStmt
    NodeTag        type;

    List       *distinctClause;
    IntoClause *intoClause;        /* target for SELECT INTO */
    List       *targetList;        /* the target list (of ResTarget) */
    List       *fromClause;        /* the FROM clause */
    Node       *whereClause;    /* WHERE qualification */
    List       *groupClause;    /* GROUP BY clauses */
    bool        groupDistinct;    /* Is this GROUP BY DISTINCT? */
    Node       *havingClause;    /* HAVING conditional-expression */
    List       *windowClause;    /* WINDOW window_name AS (...), ... */

    List       *valuesLists;    /* untransformed list of expression lists */

    List       *sortClause;        /* sort clause (a list of SortBy's) */
    Node       *limitOffset;    /* # of result tuples to skip */
    Node       *limitCount;        /* # of result tuples to return */
    LimitOption limitOption;    /* limit type */
    List       *lockingClause;    /* FOR UPDATE (list of LockingClause's) */
    WithClause *withClause;        /* WITH clause */

    SetOperation op;            /* type of set op */
    bool        all;            /* ALL specified? */
    struct SelectStmt *larg;    /* left child */
    struct SelectStmt *rarg;    /* right child */
} SelectStmt;
```
```c
// src/include/nodes/primnodes.h
// 语法树FROM clause, relation_expr经list_make1处理为List
typedef struct RangeVar {
    NodeTag        type;
    char       *catalogname;
    char       *schemaname;
    char       *relname;
    bool        inh;
    char        relpersistence;
    Alias       *alias;
    int            location;
} RangeVar;
```
`List`也是对Node的扩展，它的第一个成员也是`NodeTag`用于表示不同类型的List
```c
// src/include/nodes/pg_list.h
typedef struct List {
    // 表示不同类型的List
    NodeTag        type;
    // 当前elements的数量
    int            length;
    //  allocated length of elements[]
    int            max_length;
    // 
    ListCell   *elements;
    ListCell    initial_elements[FLEXIBLE_ARRAY_MEMBER];
} List;
```

# Analyze
`Query *parse_analyze_fixedparams(RawStmt *parseTree, ...)`实现对原始的语法树Analyze并转化为`Query`。首先了解一下`Query`结构。

## Query数据结构
```C
// src/include/nodes/parsenodes.h
typedef struct Query {
    // 取值为T_Query
    NodeTag        type;
    CmdType        commandType;
    QuerySource querySource pg_node_attr(query_jumble_ignore);
            ......
} Query;
```
### Var结构
```C
typedef struct Var {
    Expr        xpr;
    // 列属性所在表在Query->rtable中的rtindex
    int            varno;
    // 列属性在表中的编号(第几列)
    AttrNumber    varattno;

    // 列属性对应的类型
    Oid            vartype pg_node_attr(query_jumble_ignore);
    // 列属性的精度(长度)
    int32        vartypmod pg_node_attr(query_jumble_ignore);
    /* OID of collation, or InvalidOid if none */
    Oid            varcollid pg_node_attr(query_jumble_ignore);
    Bitmapset  *varnullingrels pg_node_attr(query_jumble_ignore);

    // 列属性的相对位置,和子查询相关
    // 确定列属性对应的表所在的层次(相对值)
    Index        varlevelsup;

    Index        varnosyn pg_node_attr(equal_ignore, query_jumble_ignore);
    AttrNumber    varattnosyn pg_node_attr(equal_ignore, query_jumble_ignore);

    // 列属性出现在SQL语句中的位置
    int            location;
} Var;
```

### RangeTblEntry结构
RangeTblEntry(范围表，简称RTE)描述了查询中出现的表，它通常出现在查询语句的FROM子句中，范围表中既有常规意义上的堆表，还有子查询、连接表等。
```c
typedef struct RangeTblEntry{
    NodeTag        type;
    // 范围表的类型
    RTEKind        rtekind;

    // For RTE_RELATION,普通的表
    Oid            relid;
    char        relkind;
    int            rellockmode;
    struct TableSampleClause *tablesample;
    Index        perminfoindex;

    // for RTE_SUBQUERY,子查询类型的表
    Query       *subquery;
    bool        security_barrier;

    // For RTE_JOIN,连接类型的表
    JoinType    jointype;
    int            joinmergedcols;
    // Join的表的所有列集合
    List       *joinaliasvars;
    List       *joinleftcols;
    List       *joinrightcols;

                ......
} RangeTblEntry;
```
### RangeTblRef结构
`RangeTblEntry`只保留在查询树的`Query->rtable`链表中，而链表是一个线性结构，它如何保存树状的关系代数表达式中的连接操作呢？答案是在`Query->jointree`中保存各个范围表之间的连接关系。

如果在`Query->jointree`中还保存同样的一份`RangeTblEntry`，那么一方面会造成存储的冗余，另一方面也容易产生数据不一致的问题，因此在查询树的其他任何地方都不再存放新的`RangeTblEntry`，每个范围表在Query->rtable链表中有且只能有一个，在其他地方使用到范围表都使用`RangeTblRef`来代替。`RangeTblRef`是对`RangeTblEntry`的引用，因为`RangeTblEntry`在`Query->rtable`中的位置是确定的，因此可以用它在`Query->rtable`链表中的位置`rtindex`来标识。

```C
// src/include/nodes/primnodes.h
typedef struct RangeTblRef {
    NodeTag        type;
    int            rtindex;
} RangeTblRef;
```
### 表连接
查询语句中如果显式地指定两个表之间的连接关系，例如`A LEFT JOIN B ON Pab`这种形式，用`JoinExpr`来表示
```c
typedef struct JoinExpr {
    NodeTag        type;
    // Join类型，例如JOIN_INNER等
    JoinType    jointype;
    // 是否是自然连接
    bool        isNatural;
    // 连接操作的LHS(左侧)表
    Node       *larg;
    // 连接操作的RHS(右侧)表
    Node       *rarg;
    // USING子句对应的约束条件
    List       *usingClause pg_node_attr(query_jumble_ignore);
    /* alias attached to USING clause, if any */
    Alias       *join_using_alias pg_node_attr(query_jumble_ignore);
    // on子句对应的约束条件
    Node       *quals;
    // 连接操作的投影列
    Alias       *alias pg_node_attr(query_jumble_ignore);
    // 当前JoinExpr对应的RangeTblRef->rtindex
    int            rtindex;
} JoinExpr;
```

`FromExpr`和`JoinExpr`是用来表示表之间的连接关系的结构体。通常来说，`FromExpr`中的各个表之间的连接关系是Inner Join，这样就可以在`FromExpr->fromlist`中保存任意多个表，默认是内连接的关系
```C
typedef struct FromExpr {
    NodeTag        type;
    // FromExpr中包含的表
    List       *fromlist;
    // fromlist中表间的约束条件
    Node       *quals;
} FromExpr;
```



# 查询优化

查询优化的入口在`src\backend\optimizer\plan\planner.c`的`planner`接口,它支持通过`planner_hook`来自定义优化方法，如果没有采用PG自定义的标准的查询优化函数`standard_planner`。
```C++
PlannedStmt * planner(Query *parse, const char *query_string,
                      int cursorOptions, ParamListInfo boundParams)
```

## 表达式预处理
```C
static Node * preprocess_expression(PlannerInfo *root, Node *expr, int kind)
```
预处理表达式(`preprocess_expression`函数)是对查询树(Query)中的表达式进行规范整理的过程，包括对连接产生的别名Var进行替换、对常量表达式求值、对约束条件进行拉平、为子连接(SubLink)生成执行计划等。

表达式表达数据结构
```C
// 表达式表达的父类
typedef struct Expr {
    pg_node_attr(abstract)
    NodeTag        type;
} Expr;
```

```plantuml
@startuml
BoolExpr -down-|> Expr
@enduml
```
### 常量化简
`eval_const_expressions`对含有常量的表达预先进行求值，它递归地调用了`eval_const_expressions_mutator`对不同类型的表达式处理。
```C
Node * eval_const_expressions(PlannerInfo *root, Node *node)
```
常量化简的主要的优化点有参数常量化、函数常量化、约束条件常量化3个方面
1. 参数的常量化是通过遍历参数的表达式实现的，如果发现参数表达式中全部为常量，则对参数执行预先求值。
    ```sql
    
    ```

2. 函数常量化(`simplify_function`)，如果所有的参数都是常量，则尝试预先获取函数的执行结果，并将结果常量化。
    ```sql
    
    ```

3. 


## 谓词规范
```C
// src/include/nodes/primnodes.h
// BoolExpr用于表示AND, OR和NOT的bool表达式
typedef enum BoolExprType {
    AND_EXPR, OR_EXPR, NOT_EXPR
} BoolExprType;

typedef struct BoolExpr {
    pg_node_attr(custom_read_write)
    Expr        xpr;
    BoolExprType boolop;
    List       *args;
    // token location, or -1 if unknown
    int            location;
} BoolExpr;
```
### 谓词规约
`find_duplicate_ors`可以实现谓词规约，例如,OR操作，约束条件中的NULL和FALSE是可以忽略掉的
```sql
SELECT * FROM student WHERE NULL OR FALSE OR sno=1;s

-- 谓词规约处理，等价于
SELECT * FROM student WHERE sno=1;
```
对于AND操作，如果涉及了NULL或FALSE，则实际上代表整个约束条件可以规约为FALSE
```sql
SELECT * FROM student WHERE NULL AND FALSE AND sno=1;s

-- 谓词规约处理，等价于
SELECT * FROM student WHERE FALSE;
```

相关实现源码：
```C
// src/backend/optimizer/prep/prepqual.c
// @param is_check 表示是否规约
static Expr * find_duplicate_ors(Expr *qual, bool is_check) {
    // 1. 处理OR子句处理
    if (is_orclause(qual)) {
        List       *orlist = NIL;
        ListCell   *temp;

        foreach(temp, ((BoolExpr *) qual)->args) {
            Expr       *arg = (Expr *) lfirst(temp);
            arg = find_duplicate_ors(arg, is_check);

            // 1.1 处理OR子句中的常量
            if (arg && IsA(arg, Const)) {
                Const       *carg = (Const *) arg;
                if (is_check) {
                    /* Within OR in CHECK, drop constant FALSE */
                    if (!carg->constisnull && !DatumGetBool(carg->constvalue)) continue;
                    /* Constant TRUE or NULL, so OR reduces to TRUE */
                    return (Expr *) makeBoolConst(true, false);
                } else {
                    // 如果发现了NULL或FALSE，直接跳过(丢弃)
                    if (carg->constisnull || !DatumGetBool(carg->constvalue))
                        continue;
                    // 否则，该常量是真值，该约束条件
                    return arg;
                }
            }
            orlist = lappend(orlist, arg);
        }

        // 1.2 OR拉平
        orlist = pull_ors(orlist);
        return process_duplicate_ors(orlist);
    } else if (is_andclause(qual)) {
        // 2. 处理AND子句
        List       *andlist = NIL;
        ListCell   *temp;
        foreach(temp, ((BoolExpr *) qual)->args) {
            Expr       *arg = (Expr *) lfirst(temp);
            arg = find_duplicate_ors(arg, is_check);
            
            // 2.1 处理AND子句的常量
            if (arg && IsA(arg, Const)) {
                Const       *carg = (Const *) arg;
                if (is_check) {
                    /* Within AND in CHECK, drop constant TRUE or NULL */
                    if (carg->constisnull || DatumGetBool(carg->constvalue)) continue;
                    /* Constant FALSE, so AND reduces to FALSE */
                    return arg;
                } else {
                    // 如果常量恒为True，则直接跳过
                    if (!carg->constisnull && DatumGetBool(carg->constvalue)) continue;
                    // 否则这个常量是NULL或FALSE，直接将这个约束条件规约威FALSE
                    return (Expr *) makeBoolConst(false, false);
                }
            }
            andlist = lappend(andlist, arg);
        }

        // 2.2 AND拉平
        andlist = pull_ands(andlist);

        /* AND of no inputs reduces to TRUE */
        if (andlist == NIL) return (Expr *) makeBoolConst(true, false);

        /* Single-expression AND just reduces to that expression */
        if (list_length(andlist) == 1) return (Expr *) linitial(andlist);

        /* Else we still need an AND node */
        return make_andclause(andlist);
    }
    else
        return qual;
}
```
### 谓词拉平
or谓词拉平
```C
// src/backend/optimizer/prep/prepqual.c
static List * pull_ors(List *orlist) {
    List       *out_list = NIL;
    ListCell   *arg;
    foreach(arg, orlist) {
        Node       *subexpr = (Node *) lfirst(arg);
        if (is_orclause(subexpr))
            // 对于OR之举，递归处理
            out_list = list_concat(out_list, pull_ors(((BoolExpr *) subexpr)->args));
        else
            // 非OR子句，将表达式添加到链表尾部
            out_list = lappend(out_list, subexpr);
    }
    return out_list;
}
```

AND谓词拉平
```C
// src/backend/optimizer/prep/prepqual.c
static List *pull_ands(List *andlist) {
    List       *out_list = NIL;
    ListCell   *arg;
    foreach(arg, andlist) {
        Node       *subexpr = (Node *) lfirst(arg);
        if (is_andclause(subexpr))
            // AND Bool子句，递归处理
            out_list = list_concat(out_list, pull_ands(((BoolExpr *) subexpr)->args));
        else
            // 非AND Bool子句，将表达式添加到链表尾部
            out_list = lappend(out_list, subexpr);
    }
    return out_list;
}
```

### 提取公共项
在约束条件被规约和拉平之后，可以尝试对形如`(A AND B) OR (A AND C)`的约束条件进行优化，提取出A作为公共项，相关功能实现在`process_duplicate_ors`中，实现主流程：

1. 对参数`orlist`分解，找到其中最短的子句。例如对于约束条件`(A AND B AND C)OR (A AND B) OR (A AND C AND D)`，OR操作串联了3个子约束条件，可以先尝试找到其中最短的一个`(A AND B)`，因为如果有公共因子，那么最短的那个也一定包含公共因子，通过找到最短的那个子句，在后面的操作里能减少循环的次数。

2. 以最短的约束条件为依据，提取公共项。

```c
static Expr * process_duplicate_ors(List *orlist) {
    List       *reference = NIL;
    int            num_subclauses = 0;
    ListCell   *temp;

    if (orlist == NIL) return (Expr *) makeBoolConst(false, false);

    /* Single-expression OR just reduces to that expression */
    if (list_length(orlist) == 1) return (Expr *) linitial(orlist);

    // 1. 找最短子句
    foreach(temp, orlist) {
        Expr       *clause = (Expr *) lfirst(temp);
        if (is_andclause(clause)) {
            List       *subclauses = ((BoolExpr *) clause)->args;
            int            nclauses = list_length(subclauses);
            // 比较长度，寻找比较短的约束条件
            if (reference == NIL || nclauses < num_subclauses) {
                reference = subclauses;
                num_subclauses = nclauses;
            }
        } else {
            // 不是AND类型的约束条件，全部记录下来
            // 可能是带表达式的约束条件或单个约束条件
            // 例如：(A AND B) OR A 或 (A IS NULL AND B) or A IS NULL
            reference = list_make1(clause);
            break;
        }
    }
    // 删除重复项
    reference = list_union(NIL, reference);


    // 2. 查找公共项
    //    winner表示公共项，orlist总含最短子句的AND子句或和最短子句相同的子句
    List       * winners = NIL;
    foreach(temp, reference) {
        Expr       *refclause = (Expr *) lfirst(temp);
        bool        win = true;
        ListCell   *temp2;

        foreach(temp2, orlist) {
            Expr       *clause = (Expr *) lfirst(temp2);
            if (is_andclause(clause)) {
                // 如果是AND子句，refclause是clause AND子句的子集
                if (!list_member(((BoolExpr *) clause)->args, refclause)) {
                    win = false;
                    break;
                }
            } else {
                // 非AND子句，相等是公共项
                if (!equal(refclause, clause)) {
                    win = false;
                    break;
                }
            }
        }

        if (win) winners = lappend(winners, refclause);
    }

    // 无公共项
    if (winners == NIL) return make_orclause(orlist);

    // 3. 存在公共项，提取公共项
    List       *neworlist = NIL;
    foreach(temp, orlist) {
        Expr       *clause = (Expr *) lfirst(temp);
        if (is_andclause(clause)) {
            List       *subclauses = ((BoolExpr *) clause)->args;
            // 剔除subclauses中的公共项，生成subclauses
            subclauses = list_difference(subclauses, winners);
            if (subclauses != NIL) {
                if (list_length(subclauses) == 1)
                    neworlist = lappend(neworlist, linitial(subclauses));
                else
                    neworlist = lappend(neworlist, make_andclause(subclauses));
            } else {
                // 提取后无剩余项，例如(A AND B AND C) OR (A AND B)
                neworlist = NIL;
                break;
            }
        } else {
            if (!list_member(winners, clause))
                neworlist = lappend(neworlist, clause);
            else {
                // 提取后无剩余项，例如(A AND B AND C) OR (A)
                neworlist = NIL;    /* degenerate case, see above */
                break;
            }
        }
    }

    // 将neworlist添加到winner尾部
    if (neworlist != NIL) {
        if (list_length(neworlist) == 1)
            winners = lappend(winners, linitial(neworlist));
        else
            winners = lappend(winners, make_orclause(pull_ors(neworlist)));
    }

    if (list_length(winners) == 1)
        return (Expr *) linitial(winners);
    else
        return make_andclause(pull_ands(winners));
}
```