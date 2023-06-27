ORCA的子查询执行可以采取相关执行和转Join执行两种方式，并提供了一些配置参数供用户选择。设置`optimizer_enforce_subplans`为true，不扁平化转Join行计划

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

# 数据结构表示

```C++
// Logical Apply算子,各种Apply算子的父类
class CLogicalApply : public CLogical {
protected:
	// columns used from Apply's inner child
	CColRefArray *m_pdrgpcrInner;
	// origin subquery id
	EOperatorId m_eopidOriginSubq;
};

// subquery出现在投影中，转化为下面两个Apply算子
//   其中，如果使能MaxOneRow2Assert可以去关联，转换成CLogicalLeftOuterApply
//		 否则，转换成CLogicalLeftOuterCorrelatedApply采取相关执行策略
// 	参见CSubqueryHandler::FGenerateCorrelatedApplyForScalarSubquery
class CLogicalLeftOuterApply : public CLogicalApply;
// LogicalLeftOuterApply的变体,处理物理实现需要采用相关执行策略的需求
// 例如，SELECT SUM((SELECT sum(a) FROM test_a)) FROM test_b;
// 该SQL可以转Left Outer Join来执行(CLogicalLeftOuterApply去关联),如果设置了
// 		SET optimizer_enforce_subplans=on 或
//		set optimizer_enable_assert_maxonerow=off
// 在Exploration阶段只会生成CLogicalLeftOuterCorrelatedApply二元算子采取相关执行
class CLogicalLeftOuterCorrelatedApply : public CLogicalLeftOuterApply {
private:
	BOOL m_allow_predicate_pushdown{true};
}

// subquery出现在比较谓词中，转化为下面两个Apply算子
//   其中，如果使能MaxOneRow2Assert可以去关联，转换成CLogicalInnerApply
//		 否则，转换成CLogicalInnerCorrelatedApply采取相关执行策略
// 	参见CSubqueryHandler::FGenerateCorrelatedApplyForScalarSubquery
class CLogicalInnerApply : public CLogicalApply;
class CLogicalInnerCorrelatedApply : public CLogicalInnerApply;

class CLogicalLeftAntiSemiApply : public CLogicalApply;
// Logical Apply operator used in NOT IN/ALL subqueries
class CLogicalLeftAntiSemiApplyNotIn : public CLogicalLeftAntiSemiApply;
// Logical Apply operator used in correlated execution of NOT EXISTS subqueries
class CLogicalLeftAntiSemiCorrelatedApply : public CLogicalLeftAntiSemiApply;
// Logical Apply operator used in correlated execution of NOT IN/ALL subqueries
class CLogicalLeftAntiSemiCorrelatedApplyNotIn : public CLogicalLeftAntiSemiApplyNotIn;

// Logical Apply operator used in EXISTS subquery transformations
class CLogicalLeftSemiApply : public CLogicalApply;
// Logical Apply operator used in IN/ANY subqueries
class CLogicalLeftSemiApplyIn : public CLogicalLeftSemiApply;
// Logical Apply operator used in scalar subquery transformations
class CLogicalLeftSemiCorrelatedApply : public CLogicalLeftSemiApply;
// Logical Apply operator used in scalar subquery transformations
class CLogicalLeftSemiCorrelatedApplyIn : public CLogicalLeftSemiApplyIn;


class CLogicalIndexApply : public CLogicalApply;
```

```C++
// an operator that can pass at most one row from its input
class CLogicalMaxOneRow : public CLogical;
```

```C++
// Scalar coalesce operator
class CScalarCoalesce : public CScalar
```



```sql
-- agg函数中出现子连接
-- 经过CXformGbAgg2Apply得到等价的用CLogicalLeftOuterApply(扁平化去关联)
-- 和CLogicalLeftOuterCorrelatedApply(关联执行)
-- outRel和子查询关系(SELECT sum(a) FROM test_a)表达的二元算子结构
EXPLAIN
	SELECT SUM((SELECT sum(a) FROM test_a))
	FROM test_b;
```

```
LOG:  2023-06-25 10:58:14:791149 CST,THD000,TRACE,"Xform: CXformGbAgg2Apply
Input:
+--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ]    origin: [Grp:14, GrpExpr:0]
   |--CLogicalGet "test_b" ("test_b"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}   origin: [Grp:0, GrpExpr:0]
   +--CScalarProjectList   origin: [Grp:13, GrpExpr:0]
      +--CScalarProjectElement "sum" (23)   origin: [Grp:12, GrpExpr:0]
         +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)   origin: [Grp:11, GrpExpr:0]
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
+--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ]
   |--CLogicalLeftOuterApply (Reqd Inner Cols: "sum" (22))
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
      +--CScalarProjectElement "sum" (23)
         +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)
            |--CScalarValuesList
            |  +--CScalarIdent "sum" (22)
            |--CScalarValuesList
            |--CScalarValuesList
            +--CScalarValuesList
1:
+--CLogicalGbAgg( Global ) Grp Cols: [][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ]
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
      +--CScalarProjectElement "sum" (23)
         +--CScalarAggFunc (sum , Distinct: false , Aggregate Stage: Global)
            |--CScalarValuesList
            |  +--CScalarIdent "sum" (22)
            |--CScalarValuesList
            |--CScalarValuesList
            +--CScalarValuesList
```

# 变换Transform

```C++
/**
*	transform的具体实现统一由CXformSubqueryUnnest::Transform实现，在其上派生出
*		CXformProject2Apply:
*		CXformSelect2Apply:	
*		CXformGbAgg2Apply:	
*		CXformSequenceProject2Apply
*/
class CXformSubqueryUnnest : public CXformExploration;
CXformProject2Apply::CXformProject2Apply(CMemoryPool *mp) :  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalProject(mp),
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // scalar project list
		  )) {}
CXformSelect2Apply::CXformSelect2Apply(CMemoryPool *mp) :  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalSelect(mp),
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // predicate tree
		  )){}

CXformGbAgg2Apply::CXformGbAgg2Apply(CMemoryPool *mp) :  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalGbAgg(mp),
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // project list
		  )) {}

CXformSequenceProject2Apply::CXformSequenceProject2Apply(CMemoryPool *mp) :  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalSequenceProject(mp),
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // project list
		  )) {}
```

```C++
// unnest的真实完成变换的接口,假设unary算子，例如Select/Project的salar chils含子查询
void CXformSubqueryUnnest::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								CExpression *pexpr) const {
	Transform(pxfctxt, pxfres, pexpr, false /*fEnforceCorrelatedApply*/);
	Transform(pxfctxt, pxfres, pexpr, true /*fEnforceCorrelatedApply*/);
}

void CXformSubqueryUnnest::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
            CExpression *pexpr, BOOL fEnforceCorrelatedApply) const {
	CMemoryPool *pmp = pxfctxt->Pmp();
    // 主要的扁平化的接口是PexprSubqueryUnnest
	CExpression *pexprAvoidCorrelatedApply 
		PexprSubqueryUnnest(pmp, pexpr, fEnforceCorrelatedApply);
	if (nullptr != pexprAvoidCorrelatedApply) {
		// add alternative to results
		pxfres->Add(pexprAvoidCorrelatedApply);
	}
}
```

接下来，了解扁平化实现`CXformSubqueryUnnest::PexprSubqueryUnnest`的主逻辑

```C++
CExpression * CXformSubqueryUnnest::PexprSubqueryUnnest(CMemoryPool *mp, CExpression *pexpr,
          BOOL fEnforceCorrelatedApply) {
    // 1. 如果配置SET optimizer_enforce_subplans=on;强制采取相关执行
    //    对于fEnforceCorrelatedApply为false的直接返回null,不生成可选计划
	if (GPOS_FTRACE(EopttraceEnforceCorrelatedExecution) && !fEnforceCorrelatedApply) {
		return nullptr;
	}

	CExpression *pexprOuter = (*pexpr)[0];
	CExpression *pexprScalar = (*pexpr)[1];
	pexprOuter->AddRef();

	CExpression *pexprNewOuter = nullptr;
	CExpression *pexprResidualScalar = nullptr;
    // 按照子查询出现在谓词中进行处理, 其中:
    // EsqctxtValue:子查询出现在投影列表中; EsqctxtFilter: 子查询出现在谓词中
	CSubqueryHandler::ESubqueryCtxt esqctxt = CSubqueryHandler::EsqctxtFilter;

	// 2. 调用CSubqueryHandler删除子查询并生成新的逻辑和scalar表达式
	CSubqueryHandler sh(mp, fEnforceCorrelatedApply);
	if (!sh.FProcess(pexprOuter, pexprScalar, esqctxt, &pexprNewOuter, &pexprResidualScalar)) {
		CRefCount::SafeRelease(pexprNewOuter);
		CRefCount::SafeRelease(pexprResidualScalar);
		return nullptr;
	}

	// 3. 使用新生成的逻辑和scalar表达式创建新的可选计划(alternative) 
    create a new alternative using the new logical and scalar expressions
	CExpression *pexprResult = nullptr;
	if (COperator::EopScalarProjectList == pexprScalar->Pop()->Eopid()) {
		CLogicalSequenceProject *popSeqPrj = nullptr;
		CLogicalGbAgg *popGbAgg = nullptr;
		COperator::EOperatorId op_id = pexpr->Pop()->Eopid();
		switch (op_id) {
			case COperator::EopLogicalProject:
				pexprResult = CUtils::PexprLogicalProject(
					mp, pexprNewOuter, pexprResidualScalar,false /*fNewComputedCol*/);
				break;

			case COperator::EopLogicalGbAgg:
				popGbAgg = CLogicalGbAgg::PopConvert(pexpr->Pop());
				popGbAgg->Pdrgpcr()->AddRef();
				pexprResult = CUtils::PexprLogicalGbAgg(
					mp, popGbAgg->Pdrgpcr(), pexprNewOuter, pexprResidualScalar,
					popGbAgg->Egbaggtype());
				break;

			case COperator::EopLogicalSequenceProject:
				popSeqPrj = CLogicalSequenceProject::PopConvert(pexpr->Pop());
				popSeqPrj->Pds()->AddRef();
				popSeqPrj->Pdrgpos()->AddRef();
				popSeqPrj->Pdrgpwf()->AddRef();
				pexprResult = CUtils::PexprLogicalSequenceProject(
					mp, popSeqPrj->Pds(), popSeqPrj->Pdrgpos(),
					popSeqPrj->Pdrgpwf(), pexprNewOuter, pexprResidualScalar);
				break;

			default:
				GPOS_ASSERT(!"Unnesting subqueries for an invalid operator");
				break;
		}
	} else {
		pexprResult =
			CUtils::PexprLogicalSelect(mp, pexprNewOuter, pexprResidualScalar);
	}

	// normalize resulting expression
	CExpression *pexprNormalized = CNormalizer::PexprNormalize(mp, pexprResult);
	pexprResult->Release();

	// pull up projections
	CExpression *pexprPullUpProjections =
		CNormalizer::PexprPullUpProjections(mp, pexprNormalized);
	pexprNormalized->Release();

	return pexprPullUpProjections;
}
```

## 子查询转换为Apply表达式

```C++
CSubqueryHandler::FProcess(CExpression *pexprOuter,   // logical child of a SELECT node
	CExpression *pexprScalar,  // scalar child of a SELECT node
	ESubqueryCtxt esqctxt,	   // context in which subquery occurs
	CExpression **ppexprNewOuter,  // an Apply logical expression produced as output
	CExpression **ppexprResidualScalar  // residual scalar expression produced as output
) {
    // 没有子查询, 直接返回
	if (!pexprScalar->DeriveHasSubquery()) {
		pexprScalar->AddRef();
		*ppexprResidualScalar = pexprScalar;
		return true;
	}
    // 根据Scalar的Operator Id case-by-case地处理子查询
	return FProcessScalarOperator(pexprOuter, pexprScalar, esqctxt,
								  ppexprNewOuter, ppexprResidualScalar);
}

BOOL CSubqueryHandler::FProcessScalarOperator(CExpression *pexprOuter,
         CExpression *pexprScalar, ESubqueryCtxt esqctxt,
         CExpression **ppexprNewOuter, CExpression **ppexprResidualScalar) {
	CMemoryPool *mp = m_mp;
	BOOL fSuccess = false;
	COperator::EOperatorId op_id = pexprScalar->Pop()->Eopid();
	switch (op_id) {
		case COperator::EopScalarSubquery:
			fSuccess = FRemoveScalarSubquery(pexprOuter, pexprScalar, esqctxt,
									  ppexprNewOuter, ppexprResidualScalar);
			break;
		case COperator::EopScalarSubqueryAny:
			fSuccess = FRemoveAnySubquery(pexprOuter, pexprScalar, esqctxt,
										  ppexprNewOuter, ppexprResidualScalar);
			break;
		case COperator::EopScalarSubqueryAll:
			fSuccess = FRemoveAllSubquery(pexprOuter, pexprScalar, esqctxt,
										  ppexprNewOuter, ppexprResidualScalar);
			break;
		case COperator::EopScalarSubqueryExists:
			fSuccess = FRemoveExistsSubquery(pexprOuter, pexprScalar, esqctxt,
									  ppexprNewOuter, ppexprResidualScalar);
			break;
		case COperator::EopScalarSubqueryNotExists:
			fSuccess = FRemoveNotExistsSubquery(pexprOuter, pexprScalar, esqctxt,
										 ppexprNewOuter, ppexprResidualScalar);
			break;
		case COperator::EopScalarBoolOp:
		case COperator::EopScalarProjectList:
		case COperator::EopScalarProjectElement:
		case COperator::EopScalarCmp:
		case COperator::EopScalarOp:
		case COperator::EopScalarIsDistinctFrom:
		case COperator::EopScalarNullTest:
		case COperator::EopScalarBooleanTest:
		case COperator::EopScalarIf:
		case COperator::EopScalarFunc:
		case COperator::EopScalarCast:
		case COperator::EopScalarCoerceToDomain:
		case COperator::EopScalarCoerceViaIO:
		case COperator::EopScalarArrayCoerceExpr:
		case COperator::EopScalarAggFunc:
		case COperator::EopScalarWindowFunc:
		case COperator::EopScalarArray:
		case COperator::EopScalarArrayCmp:
		case COperator::EopScalarCoalesce:
		case COperator::EopScalarCaseTest:
		case COperator::EopScalarNullIf:
		case COperator::EopScalarSwitch:
		case COperator::EopScalarSwitchCase:
		case COperator::EopScalarValuesList:
             // 递归处理,
             // project出现子连接，递归地EopScalarProjectList -> EopScalarProjectList
             //		-> EopScalarSubquery
             // 对于OR, NOT和ScalarProjectElement中的子查询在这里将
             // 将subquery context(即esqctxt设置为EsqctxtValue)
			fSuccess = FRecursiveHandler(pexprOuter, pexprScalar, esqctxt,
										 ppexprNewOuter, ppexprResidualScalar);
			break;
		default:
			GPOS_RAISE(gpopt::ExmaGPOPT, gpopt::ExmiUnexpectedOp,
					   GPOS_WSZ_LIT("Subquery in unexpected context"));
	}

	if (fSuccess) {
		// clean-up unnecessary equality operations
		CExpression *pexprPruned =
			CPredicateUtils::PexprPruneSuperfluosEquality(
				mp, *ppexprResidualScalar);
		(*ppexprResidualScalar)->Release();
		*ppexprResidualScalar = pexprPruned;

		// cleanup unncessary conjuncts
		CExpressionArray *pdrgpexpr =
			CPredicateUtils::PdrgpexprConjuncts(mp, *ppexprResidualScalar);
		(*ppexprResidualScalar)->Release();
		*ppexprResidualScalar = CPredicateUtils::PexprConjunction(mp, pdrgpexpr);
	}

	return fSuccess;
}
```

```C++
//		Replace a scalar subquery node with a column identifier, and create
//		a new Apply expression;
//
//		when subquery is defined on top of a Project node, the function simplifies
//		subquery expression by pulling-up project above subquery to facilitate
//		detecting special subquery types such as count(*) subqueries
BOOL
CSubqueryHandler::FRemoveScalarSubquery(CExpression *pexprOuter,CExpression *pexprSubquery, 
        ESubqueryCtxt esqctxt, CExpression **ppexprNewOuter, CExpression **ppexprResidualScalar) {
	CMemoryPool *pmp = m_mp;
	CScalarSubquery *popScalarSubquery = CScalarSubquery::PopConvert(pexprSubquery->Pop());
	const CColRef *pcrSubquery = popScalarSubquery->Pcr();

	SSubqueryDesc *psd = Psd(pmp, pexprSubquery, pexprOuter, pcrSubquery, esqctxt);
	if (psd->m_fReturnedPcrIsOuterRef)
	{
		// The subquery returns an outer reference. We can't simply replace the subquery with that
		// expression, because we would miss the case where the subquery is an empty table and we
		// would have to substitute the outer ref with a NULL.
		// We could use a dummy expression from the subquery to perform a check, but for now we'll
		// just give up.
		// Example: select * from foo where foo.a = (select foo.b from bar);
		return false;
	}

	BOOL fSuccess = false;
	if (psd->m_fProjectCount && !psd->m_fCorrelatedExecution) {
		// count(*)/count(Any) have special semantics: they produce '0' if their input is empty,
		// all other agg functions produce 'NULL' if their input is empty

		// for subqueries of the form (SELECT 'abc' || count(*) from X where x.i=outer.i),
		// we first create a LeftOuterApply expression to compute 'count' value and replace NULL
		// count values with '0' in the output of LOA expression,
		// we then pull the Project node below subquery to be above the LOA expression

		// create a new subquery to compute count(*) agg
		CExpression *pexprPrj = (*pexprSubquery)[0];
		CExpression *pexprPrjList = (*pexprPrj)[1];
		CExpression *pexprGbAgg = (*pexprPrj)[0];
		GPOS_ASSERT(COperator::EopLogicalGbAgg == pexprGbAgg->Pop()->Eopid());

		CScalarSubquery *popInnerSubq = GPOS_NEW(m_mp) CScalarSubquery(
			m_mp, psd->m_pcrCountAgg, false /*fGeneratedByExist*/,
			false /*fGeneratedByQuantified*/);
		pexprGbAgg->AddRef();
		CExpression *pexprNewSubq =
			GPOS_NEW(m_mp) CExpression(m_mp, popInnerSubq, pexprGbAgg);

		// unnest new subquery
		CExpression *pexprNewOuter = nullptr;
		CExpression *pexprResidualScalar = nullptr;
		psd = Psd(m_mp, pexprNewSubq, pexprOuter, popInnerSubq->Pcr(), esqctxt);
		fSuccess = FRemoveScalarSubqueryInternal(
			m_mp, pexprOuter, pexprNewSubq, EsqctxtValue, psd,
			m_fEnforceCorrelatedApply, &pexprNewOuter, &pexprResidualScalar);

		if (fSuccess)
		{
			// unnesting succeeded -- replace all occurrences of count(*) column in project list with residual expression
			pexprPrj->Pop()->AddRef();
			CExpression *pexprPrjListNew = PexprReplace(
				m_mp, pexprPrjList, psd->m_pcrCountAgg, pexprResidualScalar);
			*ppexprNewOuter = GPOS_NEW(m_mp) CExpression(
				m_mp, pexprPrj->Pop(), pexprNewOuter, pexprPrjListNew);
			*ppexprResidualScalar = CUtils::PexprScalarIdent(m_mp, pcrSubquery);
		}
	} else {
		fSuccess = FRemoveScalarSubqueryInternal(
			m_mp, pexprOuter, pexprSubquery, esqctxt, psd,
			m_fEnforceCorrelatedApply, ppexprNewOuter, ppexprResidualScalar);
	}
	return fSuccess;
}
```



```C++
BOOL CSubqueryHandler::FRemoveScalarSubqueryInternal(
	CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprSubquery,
	ESubqueryCtxt esqctxt, CSubqueryHandler::SSubqueryDesc *psd,
	BOOL fEnforceCorrelatedApply, CExpression **ppexprNewOuter,
    CExpression **ppexprResidualScalar) {
    // 1. 采取相关执行，生成correlated apply表达式
    //	 For predicate subquery可能会生成:
    //		CLogicalInnerApply(使能MaxOneRow转化为Assert) 
    //	 	CLogicalInnerCorrelatedApply(相关执行)
    // 	 For value subquey可能会生成：
    //		CLogicalLeftOuterApply(使能MaxOneRow转化为Assert
    //		CLogicalLeftOuterCorrelatedApply(相关执行)
	if (psd->m_fCorrelatedExecution || fEnforceCorrelatedApply) {
		return FGenerateCorrelatedApplyForScalarSubquery(
			mp, pexprOuter, pexprSubquery, esqctxt, psd,
			fEnforceCorrelatedApply, ppexprNewOuter, ppexprResidualScalar);
	}

	CScalarSubquery *popScalarSubquery =
		CScalarSubquery::PopConvert(pexprSubquery->Pop());
	const CColRef *colref = popScalarSubquery->Pcr();
	
    // 例如,SELECT SUM((SELECT sum(a) FROM test_a)) FROM test_b;
    // 这里的pexprSubquery是
    //		CScalarSubquery
	//	    |     +--CLogicalGbAgg(Global)
    //				+--CLogicalGet
    // pexprInner是CLogicalGbAgg
    // 生成Apply也就是将CLogicalGbAgg上拉，得到一个二元运算符，即
    //			 Apply
    //		Get			GbAgg
	CExpression *pexprInner = (*pexprSubquery)[0];
	pexprInner->AddRef();
	BOOL fSuccess = true;
    // 2. for subquery in project,生成outer apply
	if (psd->m_fValueSubquery) {
		fSuccess = FCreateOuterApply(mp, pexprOuter, pexprInner, pexprSubquery,
			nullptr /* pexprPredicate */, psd->m_fHasOuterRefs, ppexprNewOuter,
			ppexprResidualScalar, false /* not null opt for quant*/);
		if (!fSuccess) pexprInner->Release();
		return fSuccess;
	}

    // 3. for subquery in comparison predicate, 生成CLogicalInnerApply
	GPOS_ASSERT(EsqctxtFilter == esqctxt); 
	*ppexprNewOuter = CUtils::PexprLogicalApply<CLogicalInnerApply>(
		mp, pexprOuter, pexprInner, colref, pexprSubquery->Pop()->Eopid());
	*ppexprResidualScalar = CUtils::PexprScalarIdent(mp, colref);

	return fSuccess;
}
```



## Apply转Join

```C++
// Transform left outer apply into left outer join
class CXformLeftOuterApply2LeftOuterJoinNoCorrelations
	: public CXformApply2Join<CLogicalLeftOuterApply, CLogicalLeftOuterJoin>;

void CXformLeftOuterApply2LeftOuterJoinNoCorrelations::Transform(
	CXformContext *pxfctxt, CXformResult *pxfres, CExpression *pexpr) const {
	// 实际的转换动作,生成CLogicalLeftOuterJoin运算
    CreateJoinAlternative(pxfctxt, pxfres, pexpr);
}


template <class TApply, class TJoin>
class CXformApply2Join : public CXformExploration {
public:
	explicit CXformApply2Join<TApply, TJoin>(CMemoryPool *mp, BOOL) :  // pattern
		  CXformExploration(GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) TApply(mp),
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // left child
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp)),  // right child
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // predicate
			  )) {}
protected:
    /**
    *  生成候选的Join运算，得到
    *		TJoin
    *			-- pexprOuter
    *			-- pexprInner
    *			+- pexprPred
    */
    static void CreateJoinAlternative(CXformContext *pxfctxt, CXformResult *pxfres,
                      CExpression *pexprApply) {
        CMemoryPool *mp = pxfctxt->Pmp();
        CExpression *pexprOuter = (*pexprApply)[0];
        CExpression *pexprInner = (*pexprApply)[1];
        CExpression *pexprPred = (*pexprApply)[2];
            ......
        CExpression *pexprResult =
                GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) TJoin(mp),  // join operator
                    pexprOuter, pexprInner, pexprPred);
        // add alternative to results
        pxfres->Add(pexprResult);
    }
};
```

## 逻辑转物理

```C++
// Transform LeftOuter correlated apply to physical LeftOuter correlated apply
class CXformImplementLeftOuterCorrelatedApply
	: public CXformImplementCorrelatedApply<CLogicalLeftOuterCorrelatedApply,
											CPhysicalCorrelatedLeftOuterNLJoin>;

template <class TLogicalApply, class TPhysicalJoin>
class CXformImplementCorrelatedApply : public CXformImplementation {
public:
	explicit CXformImplementCorrelatedApply(CMemoryPool *mp) :  // pattern
		  CXformImplementation(GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) TLogicalApply(mp),
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // left child
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // right child
			  GPOS_NEW(mp)CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp))  // predicate
			  )) {}

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
                   CExpression *pexpr) const override {
		CMemoryPool *mp = pxfctxt->Pmp();
		CExpression *pexprLeft = (*pexpr)[0];
		CExpression *pexprRight = (*pexpr)[1];
		CExpression *pexprScalar = (*pexpr)[2];
		TLogicalApply *popApply = TLogicalApply::PopConvert(pexpr->Pop());
		CColRefArray *colref_array = popApply->PdrgPcrInner();

        		......

		// 生成物理算子
		CExpression *pexprPhysicalApply = GPOS_NEW(mp) CExpression(
			mp, GPOS_NEW(mp) TPhysicalJoin(mp, colref_array, popApply->EopidOriginSubq()),
			pexprLeft, pexprRight, pexprScalar);

		// add alternative to results
		pxfres->Add(pexprPhysicalApply);
	}
};
```

