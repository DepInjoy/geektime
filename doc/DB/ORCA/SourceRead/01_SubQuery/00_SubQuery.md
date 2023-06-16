

```C++
class CScalarProjectList : public CScalar;

//  Scalar project element operator is used to define a column reference
//  as equivalent to a scalar expression
class CScalarProjectElement : public CScalar {
private:
    // defined column reference
    CColRef *m_pcr;
};

class CScalarWindowFunc : public CScalarFunc {
private:
	// window stage
	EWinStage m_ewinstage;

	// distinct window computation
	BOOL m_is_distinct;

	/* TRUE if argument list was really '*' */
	BOOL m_is_star_arg;

	/* is function a simple aggregate? */
	BOOL m_is_simple_agg;

	// aggregate window function, e.g. count(*) over()
	BOOL m_fAgg;
};

class CScalarAggFunc : public CScalar
{
private:
	// aggregate func id
	IMDId *m_pmdidAggFunc;

	// resolved return type refers to a non-ambiguous type that was resolved during query
	// parsing if the actual return type of Agg is ambiguous (e.g., AnyElement in GPDB)
	// if resolved return type is NULL, then we can get Agg return type by looking up MD cache
	// using Agg MDId
	IMDId *m_pmdidResolvedRetType;

	// return type obtained by looking up MD cache
	IMDId *m_return_type_mdid;

	// aggregate function name
	const CWStringConst *m_pstrAggFunc;

	// distinct aggregate computation
	BOOL m_is_distinct;

	EAggfuncKind m_aggkind;

	ULongPtrArray *m_argtypes;

	// stage of the aggregate function
	EAggfuncStage m_eaggfuncstage;

	// is result of splitting aggregates
	BOOL m_fSplit;
};

// Scalar arrayref index list
class CScalarValuesList : public CScalar;

// scalar identifier operator
class CScalarIdent : public CScalar {
private:
    // column
    const CColRef *m_pcr;
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
    BOOL m_fGeneratedByQuantified;
};


//	@class:
//		CScalarSubqueryQuantified
//
//	@doc:
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

// Parent class for EXISTS/NOT EXISTS subquery operators
class CScalarSubqueryExistential : public CScalar;
class CScalarSubqueryExists : public CScalarSubqueryExistential;
class CScalarSubqueryNotExists : public CScalarSubqueryExistential;
```

```C++
//		scalar comparison operator
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
```

```sql
/**
   +--CScalarCmp (>)
      |--CScalarIdent "b" (1)
      +--CScalarSubquery["sum" (22)]
*/
EXPLAIN
	SELECT b
		FROM test_b
		WHERE b > (SELECT SUM(a) from test_a);
```


```C++
//		general scalar operation such as arithmetic and string evaluations
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
```
```sql
/**
    +--CScalarOp (+)
    |--CScalarIdent "b" (1)
    +--CScalarSubquery["sum" (22)]
*/
SELECT b+(SELECT SUM(a) from test_a)
		FROM test_b
		group by b
```


# simple project
```C++
//		Simplify existential/quantified subqueries by transforming
//		into count(*) subqueries
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
//		Transform GbAgg to Apply; this transformation is only applicable
//		to GbAgg expression with aggregate functions that have subquery arguments
class CXformGbAgg2Apply : public CXformSubqueryUnnest;

CXformGbAgg2Apply::CXformGbAgg2Apply(CMemoryPool *mp)
	:  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalGbAgg(mp),
		  GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp)
			  CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // project list
		  )) {}
```
```C++
// Simplify Project with subquery
class CXformSimplifyProjectWithSubquery : public CXformSimplifySubquery {
public:
	explicit CXformSimplifyProjectWithSubquery(CMemoryPool *mp)
		:  // pattern
		  CXformSimplifySubquery(GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CLogicalProject(mp),
			  GPOS_NEW(mp) CExpression(
				  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
			  GPOS_NEW(mp) CExpression(
				  mp, GPOS_NEW(mp) CPatternTree(mp))  // project list
			  ))
	{
	}
```

```C++
//		Simplify Select with subquery
class CXformSimplifySelectWithSubquery : public CXformSimplifySubquery {
public:
	explicit CXformSimplifySelectWithSubquery(CMemoryPool *mp)
		:  // pattern
		  CXformSimplifySubquery(GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CLogicalSelect(mp),
			  GPOS_NEW(mp) CExpression(
				  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
			  GPOS_NEW(mp) CExpression(
				  mp, GPOS_NEW(mp) CPatternTree(mp))  // predicate tree
			  ))
	{
	}
```

```C++
//		Transform Select to Apply; this transformation is only applicable
//		to a Select expression with subqueries in its scalar predicate
class CXformSelect2Apply : public CXformSubqueryUnnest
```

```C++
//		Transform Project to Apply; this transformation is only applicable
//		to a Project expression with subqueries in its scalar project list
CXformProject2Apply::CXformProject2Apply(CMemoryPool *mp)
	:  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalProject(mp),
		  GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CPatternTree(mp))  // scalar project list
		  ))
{
}
```

```C++
//		Transform MaxOneRow into LogicalAssert
class CXformMaxOneRow2Assert : public CXformExploration;
CXformMaxOneRow2Assert::CXformMaxOneRow2Assert(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CLogicalMaxOneRow(mp),
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp))))
{
}
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