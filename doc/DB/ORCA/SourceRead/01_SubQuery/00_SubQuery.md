# 数据结构

```C++
class CScalarProjectList : public CScalar;
// 用于定义等价于scalar表达式的列引用
class CScalarProjectElement : public CScalar {
private:
    // defined column reference
    CColRef *m_pcr;
};

// scalar aggregate function
class CScalarAggFunc : public CScalar
class CScalarWindowFunc : public CScalarFunc;
class CScalarFunc : public CScalar;

// Scalar arrayref index list
class CScalarValuesList : public CScalar;

// scalar identifier operator
class CScalarIdent : public CScalar {
private:
    const CColRef *m_pcr;  // column
};

// A wrapper operator for scalar constants
class CScalarConst : public CScalar;

class CScalarSubquery : public CScalar {
private:
    // computed column reference
    const CColRef *m_pcr;
    // is subquery generated from existential subquery?
    BOOL m_fGeneratedByExist;
    // is subquery generated from quantified subquery?
    BOOL m_fGeneratedByQuantified 
}
```

例如

```sql
EXPLAIN
	SELECT count((SELECT SUM(a) from test_a))
		FROM test_b
		group by b;
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



```C++
// scalar comparison operator
class CScalarCmp : public CScalar {
private:
	// metadata id in the catalog
	IMDId *m_mdid_op;
	// comparison operator name
	const CWStringConst *m_pstrOp;
	// comparison type
	IMDType::ECmpType m_comparision_type;
	// does operator return NULL on NULL input?
	BOOL m_returns_null_on_null_input;
	// is comparison commutative
	BOOL m_fCommutative;
}
/**
	EXPLAIN SELECT b FROM test_b
		WHERE b > (SELECT SUM(a) from test_a);

   +--CScalarCmp (>)
      |--CScalarIdent "b" (1)
      +--CScalarSubquery["sum" (22)]
*/
```



```C++
// general scalar operation such as arithmetic and string evaluations
class CScalarOp : public CScalar {
private:
	// metadata id in the catalog
	IMDId *m_mdid_op;
	// return type id or NULL if it can be inferred from the metadata
	IMDId *m_return_type_mdid;
	// scalar operator name
	const CWStringConst *m_pstrOp;
	// does operator return NULL on NULL input?
	BOOL m_returns_null_on_null_input;
	// is operator return type BOOL?
	BOOL m_fBoolReturnType;
	// is operator commutative
	BOOL m_fCommutative;
};
/**
	SELECT b+(SELECT SUM(a) from test_a)
		FROM test_b
		group by b

    +--CScalarOp (+)
    |--CScalarIdent "b" (1)
    +--CScalarSubquery["sum" (22)]
*/
```



```C++
// booled value scalar,for AND, OR, NOT
class CScalarBoolOp : public CScalar;
```



```C++
//		Parent class for quantified subquery operators (ALL/ANY subqueries);
//		A quantified subquery expression has two children:
//		- Logical child: the inner logical expression
//		- Scalar child:	the scalar expression in the outer expression that
//		is used in quantified comparison;
//
//		Example: SELECT * from R where a+b = ANY (SELECT c from S);
//		- logical child: (SELECT c from S)
//		- scalar child : (a+b)
class CScalarSubqueryQuantified : public CScalar;
class CScalarSubqueryAll : public CScalarSubqueryQuantified;
class CScalarSubqueryAny : public CScalarSubqueryQuantified;
```



```C++
// Parent class for EXISTS/NOT EXISTS subquery operators
class CScalarSubqueryExistential : public CScalar;
class CScalarSubqueryExists : public CScalarSubqueryExistential;
class CScalarSubqueryNotExists : public CScalarSubqueryExistential;
```



```C++
//		Scalar if operator. A case statement in SQL is represented as as
//		cascaded if statements. The format of if statement is:
//				if ------ condition
//				|-------- true value
//				|-------- false value
//		For example: (case when r.a < r.b then 10 when r.a > r.b then 20 else 15 end)
//		Is represented as if ---- r.a < r.b
//						   |----- 10
//						   |----- if ----- r.a > r.b
//								  |------- 20
//								  |------- 15
class CScalarIf : public CScalar;
```



# Transform

```C++
CXformSimplifyGbAgg::CXformSimplifyGbAgg(CMemoryPool *mp)
	: CXformExploration(// pattern
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CLogicalGbAgg(mp),
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // scalar project list
			  )) {}
```



```C++
/**
*	CXformSimplifyProjectWithSubquery : Simplify Project with subquery
*	CXformSimplifySelectWithSubquery : Simplify Select with subquery
*	
*	具体的Transform动作统一由CXformSimplifySubquery::Transform实现
*		对于existential/quantified subqueries将其转化为COUT(*)计算
*/
class CXformSimplifyProjectWithSubquery : public CXformSimplifySubquery {
public:
	explicit CXformSimplifyProjectWithSubquery(CMemoryPool *mp) :  // pattern
		  CXformSimplifySubquery(GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CLogicalProject(mp),
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // project list
			  )) {}
};

class CXformSimplifySelectWithSubquery : public CXformSimplifySubquery {
public:
	explicit CXformSimplifySelectWithSubquery(CMemoryPool *mp) :  // pattern
		  CXformSimplifySubquery(GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CLogicalSelect(mp),
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // predicate tree
			  )) {}
};

class CXformSimplifySubquery : public CXformExploration;
//		Transform existential subqueries to count(*) subqueries;
//		the function returns true if transformation succeeded
BOOL CXformSimplifySubquery::FSimplifyExistential(CMemoryPool *mp,
        CExpression *pexprScalar, CExpression **ppexprNewScalar);
//		Transform quantified subqueries to count(*) subqueries;
//		the function returns true if transformation succeeded
BOOL CXformSimplifySubquery::FSimplifyQuantified(CMemoryPool *mp,
    	CExpression *pexprScalar, CExpression **ppexprNewScalar);
```



```C++
//		Transform Project to Apply; this transformation is only applicable
//		to a Project expression with subqueries in its scalar project list
class CXformProject2Apply : public CXformSubqueryUnnest;
CXformProject2Apply::CXformProject2Apply(CMemoryPool *mp) :  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CLogicalProject(mp),
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // scalar project list
		  )) {}

// Transform GbAgg to Apply; this transformation is only applicable
// to GbAgg expression with aggregate functions that have subquery arguments
class CXformGbAgg2Apply : public CXformSubqueryUnnest
CXformGbAgg2Apply::CXformGbAgg2Apply(CMemoryPool *mp) :  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CLogicalGbAgg(mp),
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // project list
		  )) {}

//		Transform Select to Apply; this transformation is only applicable
//		to a Select expression with subqueries in its scalar predicate
class CXformSelect2Apply : public CXformSubqueryUnnest;
CXformSelect2Apply::CXformSelect2Apply(CMemoryPool *mp)  :  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CLogicalSelect(mp),
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // predicate tree
		  )) {}
```



```C++
// Transform MaxOneRow into LogicalAssert
class CXformMaxOneRow2Assert : public CXformExploration;
CXformMaxOneRow2Assert::CXformMaxOneRow2Assert(CMemoryPool *mp)
	: CXformExploration(// pattern
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CLogicalMaxOneRow(mp),
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)))) { }
```





# 转化为subplan

```C++
/**
 *  Helper to build subplans of different types
 *  CTranslatorExprToDXL::BuildSubplans
 *      -> CTranslatorExprToDXL::BuildSubplansForCorrelatedLOJ
 *              see CTranslatorExprToDXL::Edxlsubplantype
 *          -> CTranslatorExprToDXL::PdxlnQuantifiedSubplan,
 *                  if quantified (ANY/ALL) subqueries
 *          -> CTranslatorExprToDXL::PdxlnExistentialSubplan
 *                  if existential subquery
 *          -> CTranslatorExprToDXL::BuildScalarSubplans
 *                  if scalar subquery
*/
void
CTranslatorExprToDXL::BuildSubplans(
	CExpression *pexprCorrelatedNLJoin, CDXLColRefArray *dxl_colref_array,
	CDXLNode ** ppdxlnScalar,  // output: scalar condition after replacing inner child reference with subplan
	CDistributionSpecArray *pdrgpdsBaseTables, ULONG *pulNonGatherMotions,
	BOOL *pfDML)


// Helper to build subplans from correlated LOJ
void CTranslatorExprToDXL::BuildSubplansForCorrelatedLOJ(
	CExpression *pexprCorrelatedLOJ, CDXLColRefArray *dxl_colref_array,
	CDXLNode ** ppdxlnScalar,  // output: scalar condition after replacing inner child reference with subplan
	CDistributionSpecArray *pdrgpdsBaseTables, ULONG *pulNonGatherMotions, BOOL *pfDML)
```