

PG中语法树采用统一的形式，都是基于Node数据结构的"扩展"，该结构中只包含`NodeTag`成员变量，`NodeTag`是枚举类型。在`src\include\nodes\nodes.h`中有Node的定义

```c
typedef struct Node
{
	NodeTag		type;
} Node;
```

在Parser中采用`makeNode`来创建Node,相关实现位于`src\include\nodes\nodes.h`

```c
#define makeNode(_type_)		((_type_ *) newNode(sizeof(_type_),T_##_type_))
```

# 表的列属性

`Var` 结构体定义在`src\include\nodes\primnodes.h`，表示查询中涉及的表的列属性，在 SQL 语句中，投影的列属性、约束条件中的列属性都是通过`Var`来表示的，在语法分析阶段会将列属性用`ColumnRef` 结构体来表示，在语义分析阶段会将语法树中的 `ColumnRef` 替换成 `Var` 用来表示 个列属性

```c
// 查询中涉及的列属性
typedef struct Var
{
	Expr		xpr;
	// 列属性所在表编号，Query->rtable中的rtindex
	Index		varno;			/* index of this var's relation in the range
								 * table, or INNER_VAR/OUTER_VAR/INDEX_VAR */
	
	// 列属性在表中的编号(第几列)
	AttrNumber	varattno;		/* attribute number of this var, or zero for
								 * all */
	// 列属性对应的类型
	Oid			vartype;		/* pg_type OID for the type of this var */
	// 列属性的精度(长度)
	int32		vartypmod;		/* pg_attribute typmod value */
	
	// 
	Oid			varcollid;		/* OID of collation, or InvalidOid if none */
	
	// 列属性对应的表所在层次，相对值，和子查询相关
	Index		varlevelsup;	/* for subquery variables referencing outer
								 * relations; 0 in a normal var, >0 means N
								 * levels up */
	
	// 	varnoold/varoattno一般与varno/varattno相同,在等价变化过程中
	//	varno/varattno可能会变化，该变量记录变化前的值
	// varno的初始值，
	Index		varnoold;		/* original value of varno, for debugging */
	// varattno的初始值
	AttrNumber	varoattno;		/* original value of varattno */
	
	// 列属性出现在SQL语句中的位置
	int			location;		/* token location, or -1 if unknown */
} Var;
```



`RangeTblEntry`范围表简称RTE，描述查询中出现的表，定义位于`src\include\nodes\parsenodes.h`

```c
// 范围表简称RTE,含常规意义的堆表,子查询、连接表等
// 只保留在Query->rtable链表中,线性结构，只有一个,避免数据不一致,其他采用RangeTblRef
// 树状关系表达式连接操作保存在Query->jointree中,保存各个范围表间连接关系
typedef struct RangeTblEntry
```

相关定义位于`src\include\nodes\primnodes.h`

```c
/**
 * RangeTblEntry的引用
*/
typedef struct RangeTblRef
{
	NodeTag		type;
	// 对应的RangeTblEntry在Query->rtable中的位置
	int			rtindex;
} RangeTblRef;
```



`JoinExpr`和`FromExpr`都是用来表示表间连接关系，通常`FromExpr`中各表之间是`Inner Join`的连接关系。

```c
/**
 * 	定义两个表的连接关系
*/
typedef struct JoinExpr
```



```c
/*----------
 * FromExpr - represents a FROM ... WHERE ... construct
 * 表示表姐连接关系，通常各表间是Inner Join连接关系
 */
typedef struct FromExpr
```



`Query`是查询优化模块的输入参数，来源于语法分析模块，SQL经过执行词法分析、语法分析和语义分析后生成一颗查询树，该查询树在PostgreSQL中用`Query`结构来表示。

查询优化模块在获取到查询树后，开始进行逻辑优化，也就是查询树等价变换，将其重写成一颗新的查询树，这个新查询树作为物理优化的输入参数，进行物理优化。

相关定义位于`src\include\nodes\parsenodes.h`

```c

typedef struct Query
```

