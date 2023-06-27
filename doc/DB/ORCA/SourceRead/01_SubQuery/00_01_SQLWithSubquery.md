# Agg+子连接

```sql
EXPLAIN
	SELECT count((SELECT SUM(a) from test_a))
		FROM test_b
		group by b;
```
设置`optimizer_enforce_subplans`为true，不扁平化转Join行计划
```sql
-- Enforce correlated execution in the optimizer
SET optimizer_enforce_subplans=on;
```
对于可能会返回多行的子链接，如果将`optimizer_enable_assert_maxonerow`设置为false，禁用将`MaxOneRows`转化成`Assert`生成的计划也不会扁平化。

```sql
-- disable Assert MaxOneRow plans
set optimizer_enable_assert_maxonerow=off;
```

例如`SELECT SUM((SELECT a FROM test_a)) FROM test_b`生成Apply二元运算只有`CLogicalLeftOuterCorrelatedApply`。而对于子连接不会返回多行的子链接，依然会生成`SELECT SUM((SELECT sum(a) FROM test_a)) FROM test_b;`会生成`CLogicalLeftOuterCorrelatedApply`和`CLogicalLeftOuterApply`二元运算，后者可以实现扁平化，生成Join计划。



采取相关执行，也就是子连接得到subplan广播+物化的方式， 可以得到下面的执行计划，

```
 Gather Motion 3:1  (slice1; segments: 3)  (cost=0.00..1324044.43 rows=2 width=8)
   ->  HashAggregate  (cost=0.00..1324044.43 rows=1 width=8)
         Group Key: test_b.b
         ->  Redistribute Motion 3:3  (slice2; segments: 3)  (cost=0.00..1324044.39 rows=334 width=8)
               Hash Key: test_b.b
               ->  Seq Scan on test_b  (cost=0.00..1324044.39 rows=334 width=8)
                     SubPlan 1
                       ->  Materialize  (cost=0.00..431.00 rows=2 width=4)
                             ->  Broadcast Motion 3:3  (slice3; segments: 3)  (cost=0.00..431.00 rows=2 width=4)
                                   ->  Seq Scan on test_a  (cost=0.00..431.00 rows=1 width=4)
 Optimizer: Pivotal Optimizer (GPORCA)
```

```
Algebrized query: 
+--CLogicalGbAgg( Global ) Grp Cols: ["b" (1)][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ] 
   |--CLogicalGet "test_b" ("test_b"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}
   +--CScalarProjectList
      +--CScalarProjectElement "count" (23)
         +--CScalarAggFunc (count , Distinct: false , Aggregate Stage: Global)
            |--CScalarValuesList
            |  +--CScalarSubquery["sum" (22)]
            |     +--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ] 
            |        |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}
            |        +--CScalarProjectList
            |           +--CScalarProjectElement "sum" (22)
            |              +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)
            |                 |--CScalarValuesList
            |                 |  +--CScalarIdent "a" (11)
            |                 |--CScalarValuesList
            |                 |--CScalarValuesList
            |                 +--CScalarValuesList
            |--CScalarValuesList
            |--CScalarValuesList
            +--CScalarValuesList
```

> 理论上，我们可以将所有的子查询转换成 Apply 算子，一个通用的方法如下：
> 1. 如果某个算子的表达式中出现了子查询，我们就把这个子查询提取到该算子下面（留下一个子查询的结果变量），构成一个$\mathcal{A}^{LOJ}$算子。如果不止一个子查询，则会产生多个$\mathcal{A}^{LOJ}$。必要的时候加上`max1row`算子。
> 2. 然后应用其他一些规则，将$\mathcal{A}^{LOJ}$转换成$\mathcal{A}^{\times}$, $\mathcal{A}^{\exists}$, $\mathcal{A}^{\nexists}$。
> 来自知乎：https://zhuanlan.zhihu.com/p/60380557

```
2023-06-05 14:26:01:781043 CST,THD000,TRACE,"Xform: CXformGbAgg2Apply
Input:
+--CLogicalGbAgg( Global ) Grp Cols: ["b" (1)][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ]    origin: [Grp:14, GrpExpr:0]
   |--CLogicalGet "test_b" ("test_b"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}   origin: [Grp:0, GrpExpr:0]
   +--CScalarProjectList   origin: [Grp:13, GrpExpr:0]
      +--CScalarProjectElement "count" (23)   origin: [Grp:12, GrpExpr:0]
         +--CScalarAggFunc (count , Distinct: false , Aggregate Stage: Global)   origin: [Grp:11, GrpExpr:0]
            |--CScalarValuesList   origin: [Grp:10, GrpExpr:0]
            |  +--CScalarSubquery["sum" (22)]   origin: [Grp:9, GrpExpr:0]
            |     +--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ]    origin: [Grp:8, GrpExpr:0]
            |        |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}   origin: [Grp:1, GrpExpr:0]
            |        +--CScalarProjectList   origin: [Grp:7, GrpExpr:0]
            |           +--CScalarProjectElement "sum" (22)   origin: [Grp:6, GrpExpr:0]
            |              +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)   origin: [Grp:5, GrpExpr:0]
            |                 |--CScalarValuesList   origin: [Grp:3, GrpExpr:0]
            |                 |  +--CScalarIdent "a" (11)   origin: [Grp:2, GrpExpr:0]
            |                 |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
            |                 |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
            |                 +--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
            |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
            |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
            +--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
Output:
Alternatives:
0:
+--CLogicalGbAgg( Global ) Grp Cols: ["b" (1)][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ]
   |--CLogicalLeftOuterCorrelatedApply (Reqd Inner Cols: "sum" (22))
   |  |--CLogicalGet "test_b" ("test_b"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}   origin: [Grp:0, GrpExpr:0]
   |  |--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ]
   |  |  |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}   origin: [Grp:1, GrpExpr:0]
   |  |  +--CScalarProjectList   origin: [Grp:7, GrpExpr:0]
   |  |     +--CScalarProjectElement "sum" (22)   origin: [Grp:6, GrpExpr:0]
   |  |        +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)   origin: [Grp:5, GrpExpr:0]
   |  |           |--CScalarValuesList   origin: [Grp:3, GrpExpr:0]
   |  |           |  +--CScalarIdent "a" (11)   origin: [Grp:2, GrpExpr:0]
   |  |           |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
   |  |           |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
   |  |           +--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
   |  +--CScalarConst (1)
   +--CScalarProjectList
      +--CScalarProjectElement "count" (23)
         +--CScalarAggFunc (count , Distinct: false , Aggregate Stage: Global)
            |--CScalarValuesList
            |  +--CScalarIdent "sum" (22)
            |--CScalarValuesList
            |--CScalarValuesList
            +--CScalarValuesList
```

如果不转Apply，进行agg拆分
```
2023-06-05 14:26:01:781243 CST,THD000,TRACE,"Xform: CXformSplitGbAgg
Input:
+--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ]    origin: [Grp:8, GrpExpr:0]
   |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}   origin: [Grp:1, GrpExpr:0]
   +--CScalarProjectList   origin: [Grp:7, GrpExpr:0]
      +--CScalarProjectElement "sum" (22)   origin: [Grp:6, GrpExpr:0]
         +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)   origin: [Grp:5, GrpExpr:0]
            |--CScalarValuesList   origin: [Grp:3, GrpExpr:0]
            |  +--CScalarIdent "a" (11)   origin: [Grp:2, GrpExpr:0]
            |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
            |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
            +--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
Output:
Alternatives:
0:
+--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 1 ]
   |--CLogicalGbAgg( Local ) Grp Cols: [][Local], Minimal Grp Cols: [], Generates Duplicates :[ 1 ]
   |  |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}   origin: [Grp:1, GrpExpr:0]
   |  +--CScalarProjectList
   |     +--CScalarProjectElement "ColRef_0024" (24)
   |        +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Local)
   |           |--CScalarValuesList   origin: [Grp:3, GrpExpr:0]
   |           |  +--CScalarIdent "a" (11)   origin: [Grp:2, GrpExpr:0]
   |           |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
   |           |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
   |           +--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
   +--CScalarProjectList
      +--CScalarProjectElement "sum" (22)
         +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)
            |--CScalarValuesList
            |  +--CScalarIdent "ColRef_0024" (24)
            |--CScalarValuesList
            |--CScalarValuesList
            +--CScalarValuesList
```

最终选取的物理计划：
```
Physical plan:
+--CPhysicalMotionGather(master)   rows:2   width:12  rebinds:1   cost:1324044.434251   origin: [Grp:8, GrpExpr:7]
   +--CPhysicalHashAgg( Global ) Grp Cols: ["b" (1)], Minimal Grp Cols:["b" (1)], Generates Duplicates :[ 0 ]    rows:2   width:12  rebinds:1   cost:1324044.434192   origin: [Grp:8, GrpExpr:6]
      |--CPhysicalMotionHashDistribute HASHED: [ CScalarIdent "b" (1), nulls colocated ], opfamilies: (1977,1.0),   rows:1000   width:8  rebinds:1   cost:1324044.393890   origin: [Grp:10, GrpExpr:2]
      |  +--CPhysicalCorrelatedLeftOuterNLJoin   rows:1000   width:8  rebinds:1   cost:1324044.385544   origin: [Grp:10, GrpExpr:1]
      |     |--CPhysicalTableScan "test_b" ("test_b")   rows:2   width:38  rebinds:1   cost:431.000017   origin: [Grp:0, GrpExpr:1]
      |     |--CPhysicalSpool (Streaming)   rows:2   width:34  rebinds:1   cost:431.000256   origin: [Grp:1, GrpExpr:3]
      |     |  +--CPhysicalMotionBroadcast    rows:2   width:34  rebinds:1   cost:431.000248   origin: [Grp:1, GrpExpr:2]
      |     |     +--CPhysicalTableScan "test_a" ("test_a")   rows:2   width:34  rebinds:1   cost:431.000025   origin: [Grp:1, GrpExpr:1]
      |     +--CScalarConst (1)   origin: [Grp:9, GrpExpr:0]
      +--CScalarProjectList   origin: [Grp:15, GrpExpr:0]
         +--CScalarProjectElement "count" (22)   origin: [Grp:14, GrpExpr:0]
            +--CScalarAggFunc (count , Distinct: false , Aggregate Stage: Global)   origin: [Grp:13, GrpExpr:0]
               |--CScalarValuesList   origin: [Grp:12, GrpExpr:0]
               |  +--CScalarIdent "a" (11)   origin: [Grp:11, GrpExpr:0]
               |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
               |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
               +--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
```

# Having+子连接

```sql
explain
SELECT b
		FROM test_b
		group by b
		having count(b) > (select sum(a) from test_a);

-- 将groupby+count(b)计算结果作为关系
-- 之后用count(b)>sublink 进行过滤(Selct)
-- translate query等价于
explain
select b (
    SELECT b, count(b) as cnt_b
        from test_b
        group by b
) where cnt_b > (select sum(a) from test_a);
```

最初的
```
Algebrized query: 
+--CLogicalSelect
   |--CLogicalGbAgg( Global ) Grp Cols: ["b" (1)][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ] 
   |  |--CLogicalGet "test_b" ("test_b"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}
   |  +--CScalarProjectList
   |     +--CScalarProjectElement "count" (11)
   |        +--CScalarAggFunc (count , Distinct: false , Aggregate Stage: Global)
   |           |--CScalarValuesList
   |           |  +--CScalarIdent "b" (1)
   |           |--CScalarValuesList
   |           |--CScalarValuesList
   |           +--CScalarValuesList
   +--CScalarCmp (>)
      |--CScalarIdent "count" (11)
      +--CScalarSubquery["sum" (23)]
         +--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ] 
            |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (12), "b" (13), "c" (14), "d" (15), "ctid" (16), "xmin" (17), "cmin" (18), "xmax" (19), "cmax" (20), "tableoid" (21), "gp_segment_id" (22)] Key sets: {[4,10]}
            +--CScalarProjectList
               +--CScalarProjectElement "sum" (23)
                  +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)
                     |--CScalarValuesList
                     |  +--CScalarIdent "a" (12)
                     |--CScalarValuesList
                     |--CScalarValuesList
                     +--CScalarValuesList
```

# DISTINCT+子连接
```sql
EXPLAIN
	SELECT distinct (SELECT SUM(a) from test_a)
		FROM test_b;
```

```
Algebrized query: 
+--CLogicalGbAgg( Global ) Grp Cols: ["sum" (23)][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ] 
   |--CLogicalProject
   |  |--CLogicalGet "test_b" ("test_b"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}
   |  +--CScalarProjectList
   |     +--CScalarProjectElement "sum" (23)
   |        +--CScalarSubquery["sum" (22)]
   |           +--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ] 
   |              |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}
   |              +--CScalarProjectList
   |                 +--CScalarProjectElement "sum" (22)
   |                    +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)
   |                       |--CScalarValuesList
   |                       |  +--CScalarIdent "a" (11)
   |                       |--CScalarValuesList
   |                       |--CScalarValuesList
   |                       +--CScalarValuesList
   +--CScalarProjectList
```

```
2023-06-06 18:20:50:422707 CST,THD000,TRACE,"Xform: CXformSplitGbAgg
Input:
+--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ]    origin: [Grp:8, GrpExpr:0]
   |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}   origin: [Grp:1, GrpExpr:0]
   +--CScalarProjectList   origin: [Grp:7, GrpExpr:0]
      +--CScalarProjectElement "sum" (22)   origin: [Grp:6, GrpExpr:0]
         +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)   origin: [Grp:5, GrpExpr:0]
            |--CScalarValuesList   origin: [Grp:3, GrpExpr:0]
            |  +--CScalarIdent "a" (11)   origin: [Grp:2, GrpExpr:0]
            |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
            |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
            +--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
Output:
Alternatives:
0:
+--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 1 ]
   |--CLogicalGbAgg( Local ) Grp Cols: [][Local], Minimal Grp Cols: [], Generates Duplicates :[ 1 ]
   |  |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}   origin: [Grp:1, GrpExpr:0]
   |  +--CScalarProjectList
   |     +--CScalarProjectElement "ColRef_0024" (24)
   |        +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Local)
   |           |--CScalarValuesList   origin: [Grp:3, GrpExpr:0]
   |           |  +--CScalarIdent "a" (11)   origin: [Grp:2, GrpExpr:0]
   |           |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
   |           |--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
   |           +--CScalarValuesList   origin: [Grp:4, GrpExpr:0]
   +--CScalarProjectList
      +--CScalarProjectElement "sum" (22)
         +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)
            |--CScalarValuesList
            |  +--CScalarIdent "ColRef_0024" (24)
            |--CScalarValuesList
            |--CScalarValuesList
            +--CScalarValuesList
```