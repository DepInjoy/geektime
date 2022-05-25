PostgreSQl将语句划分为两类：
- 可优化语句(Optimizable Statements),
- 不可优化语句(Non-Optimizable Statements), 通常工具语句(Utility Statement)属于该范畴。此类语句不会经过analyze、rewriting和planning三个阶段。

# 语法树
## 基表数据结构
相关结果定义位于`src\include\nodes\primnodes.h`或`src\include\nodes\parsenodes.h`。
- `struct Query`分析器对解析器产出的语法解析树进行语义分析，产出一棵查询树, 查询树的根节点用该Query结构表示。
- `struct SelectStmt`SELECT型查询语句。
- `struct TargetEntry`描述目标列中的每一列。
- `struct FromExpr`
- `RangeTblEntry/RangeTblRef`描述范围表，通常SQL中FROM子句给出的语法元素。
- `JoinExpr`描述语句的Join操作。
- `RangeSubselect`From语句总子查询
- `SubLink`子连接
- `SubPlan`子查询计划，查询优化过程中，对于满足优化条件的子连接进行Join化处理，对于不可Join化的子连接以原型方式存在于查询树中。为了在查询计划构建过程中表示此类子连接，PG以子查询来描述查询计划构建过程中子连接中的子查询对象，即`SubLink`中的`subselect`描述的语法对象。
# 查询优化
## 工具类语句查询
`ProcessUtility`实现工具类语句处理。用户可以通过`ProcessUtility_hook`插件来自定义实现，如果没有设置，调用PG提供的`standard_ProcessUtility`语句语句类型进行分类处理,相应的实现位于`src\backend\tcop\utility.c`。
```c
void
ProcessUtility(PlannedStmt *pstmt,
			   const char *queryString,
			   bool readOnlyTree,
			   ProcessUtilityContext context,
			   ParamListInfo params,
			   QueryEnvironment *queryEnv,
			   DestReceiver *dest,
			   QueryCompletion *qc)
{
            ......

	if (ProcessUtility_hook)
		(*ProcessUtility_hook) (pstmt, queryString, readOnlyTree,
								context, params, queryEnv,
								dest, qc);
	else
		standard_ProcessUtility(pstmt, queryString, readOnlyTree,
								context, params, queryEnv,
								dest, qc);
}
```

## 查询类语句处理
普通类查询语句经过语法分析和词法分析得到一颗原始的语法树，之后原始的语法树转化为查询语法树，在这个过程中进行语义检查，再对查询语法树进行逻辑优化、物理优化、查询计划创建等。
` parse_analyze_XXX`语句语法树上节点类型分别完成INSTER、DELETE、UPDATE、SELECT等子句分析，相关实现位于`src\backend\parser\analyze.c`。
```
Query *
parse_analyze_fixedparams(RawStmt *parseTree, const char *sourceText,
			  const Oid *paramTypes, int numParams,
			  QueryEnvironment *queryEnv)
```

`pg_rewrite_query`主要使用`pg_rewrite`和`pg_rules`等系统表中提供的重写规则对原始的语法树进行重写。相关实现位于`src\backend\tcop\postgres.c`。

```c
List * pg_rewrite_query(Query *query)
```
`transformStmt`依据查询语句类型将相应语法树转化到查询树的转换, 相关实现位于`src\backend\commands\analyze.c`
```c
Query *
transformStmt(ParseState *pstate, Node *parseTree)
```

`pull_up_sublinks`对IN和EXISTS类型的子连接优化处理，其会调用`convert_ANY_sublink_to_join`和`convert_EXISTS_sublink_to_join`。
```c
void pull_up_sublinks(PlannerInfo *root)
```
- `convert_ANY_sublink_to_join`将IN类型的子连接转化为Semi-Join。
    ```c
    JoinExpr * convert_ANY_sublink_to_join(PlannerInfo *root, SubLink *sublink,
            Relids available_rels)
    ```
- `convert_EXISTS_sublink_to_join`将EXISTS子查询转化为Semi-Join。
    ```c
    JoinExpr *
    convert_EXISTS_sublink_to_join(PlannerInfo *root, SubLink *sublink,
            bool under_not, Relids available_rels)
    ```

`pull_up_subqueries`对子查询上提，将子查询基表(Base Relation)上提至父查询，使得子查询基表有机会于父查询基表合并，由查询引擎统一处理。
```c
void pull_up_subqueries(PlannerInfo *root)
```

`preprocess_expression`对查询树中表达式预处理，例如表达式规则化、常量表达式求值优化等。
```c
static Node * preprocess_expression(PlannerInfo *root, Node *expr, int kind)
```
完成表达式优化处理后，对查询约束条件进行相关优化处理，如约束条件下推，约束条件合并、推导和无效约束条件移除等。之后查询引擎将优化处理后的约束条件绑定到相应的基表上，即约束条件分配(Distribute the Restriction Clauses).完成这些操作开始物理优化(Physical Optimization).

`SS_process_sublinks`进行查询树SubLink类型节点到SubPlan类型节点的转换。
```C
Node * SS_process_sublinks(PlannerInfo *root, Node *expr, bool isQual)
```
## 物理优化
为了正确且高效计算不同查询访问路径下的查询代价，查询引擎依据基表之上存在的约束条件，估算出获取满足该约束条件的元组所需要的I/O和CPU代价。以概率论和统计分析为工具，通过获取元数据表`pg_statistic`中统计信息计算出满足约束条件的元组占整个元组的比重，来估算该约束条件下元组数量，通常使用选择率(Selectivity)来描述比重。

完成基表物理参数和约束条件设置后，查询引擎考察各个基表所能形成的连接条件，若可以形成连接关系，则尝试确立连接类型并完成对此连接条件下的查询代价估算。物理连接路径有Nestlooped Join、HashJoin和SortMergeJoin。

PG采用动态规划来求解最优的查询访问路径，基表数量太多会导致最优搜索空间以指数级增长，传统动态规划无法求解，PG引入基因遗传算法(Genetic Algorithm)来加速最优解的求解。


## 创建查询计划
`create_plan`依据查询访问路径中节点类型分类创建对应查询计划
- `create_seqscan_plan`创建顺序扫描查询计划
- `create_mergejoin_plan`创建MergeJoin查询计划
- `create_hashjoin_plan`创建HashJoin查询计划
- `create_nestloop_plan`创建Nestloop Join查询计划

```c
Plan * create_plan(PlannerInfo *root, Path *best_path)
```

# 查询计划缓存
查询计划缓存机制(Query Plans Caching或Query Paths Caching):数据库管理系统提供原生的最右查询访问路径代价缓存机制或第三方查询计划缓存解决方案，注意事项：
- 查询语句需要满足一定条件，如不含易失函数，语句设计的基表定义发生变化后的正确处理等，才可以使用缓存机制，否则可能导致查询结果不正确。

相关实现位于`src\backend\utils\cache\plancache.c`.


