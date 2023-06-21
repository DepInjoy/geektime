#  数据结构表示

## 存在性测试

Existential Test，如`(NOT) EXISTS Subquery`。

```sql
EXPLAIN
SELECT * FROM test_a WHERE NOT EXISTS(
	SELECT a FROM test_b WHERE test_a.a=test_b.a);
```

```
Algebrized query: 
+--CLogicalSelect
   |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}
   +--CScalarSubqueryNotExists
      +--CLogicalGet "test_b" ("test_b"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}
```

```sql
EXPLAIN
SELECT * FROM test_a WHERE
	EXISTS(SELECT a FROM test_b) or
		EXISTS(SELECT a FROM test_b WHERE a > 10);
```

```
Algebrized query: 
+--CLogicalSelect
   |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}
   +--CScalarBoolOp (EboolopOr)
      |--CScalarSubqueryExists
      |  +--CLogicalGet "test_b" ("test_b"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}
      +--CScalarSubqueryExists
         +--CLogicalSelect
            |--CLogicalGet "test_b" ("test_b"), Columns: ["a" (22), "b" (23), "c" (24), "d" (25), "ctid" (26), "xmin" (27), "cmin" (28), "xmax" (29), "cmax" (30), "tableoid" (31), "gp_segment_id" (32)] Key sets: {[4,10]}
            +--CScalarCmp (>)
               |--CScalarIdent "a" (22)
               +--CScalarConst (10)
```

```sql
EXPLAIN SELECT * FROM test_a WHERE
	EXISTS(SELECT a FROM test_b) and
		EXISTS(SELECT a FROM test_b WHERE a <= 10);
```

```
Algebrized query: 
+--CLogicalSelect
   |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}
   +--CScalarBoolOp (EboolopAnd)
      |--CScalarSubqueryExists
      |  +--CLogicalGet "test_b" ("test_b"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}
      +--CScalarSubqueryExists
         +--CLogicalSelect
            |--CLogicalGet "test_b" ("test_b"), Columns: ["a" (22), "b" (23), "c" (24), "d" (25), "ctid" (26), "xmin" (27), "cmin" (28), "xmax" (29), "cmax" (30), "tableoid" (31), "gp_segment_id" (32)] Key sets: {[4,10]}
            +--CScalarCmp (<=)
               |--CScalarIdent "a" (22)
               +--CScalarConst (10)
```

## 集合比较

Quantified Comparision，如`T.a = ANY/ ALL Subquery, T.a NOT IN Subquery`

```sql
EXPLAIN
SELECT * FROM test_a WHERE a not in (
	SELECT a FROM test_b WHERE test_a.a>test_b.a);
```

```
Algebrized query: 
+--CLogicalSelect
   |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}
   +--CScalarSubqueryAll(<>)["a" (11)]
      |--CLogicalSelect
      |  |--CLogicalGet "test_b" ("test_b"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}
      |  +--CScalarCmp (>)
      |     |--CScalarIdent "a" (0)
      |     +--CScalarIdent "a" (11)
      +--CScalarIdent "a" (0)
```



```sql
EXPLAIN
SELECT * FROM test_a WHERE a in (
	SELECT a FROM test_b WHERE test_a.a>test_b.a);
```

```
Algebrized query: 
+--CLogicalSelect
   |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}
   +--CScalarSubqueryAny(=)["a" (11)]
      |--CLogicalSelect
      |  |--CLogicalGet "test_b" ("test_b"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}
      |  +--CScalarCmp (>)
      |     |--CScalarIdent "a" (0)
      |     +--CScalarIdent "a" (11)
      +--CScalarIdent "a" (0)
```



```sql
EXPLAIN
SELECT * FROM test_a WHERE a = any(
	SELECT a FROM test_b WHERE test_a.a>test_b.a);
```

```
Algebrized query: 
+--CLogicalSelect
   |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}
   +--CScalarSubqueryAny(=)["a" (11)]
      |--CLogicalSelect
      |  |--CLogicalGet "test_b" ("test_b"), Columns: ["a" (11), "b" (12), "c" (13), "d" (14), "ctid" (15), "xmin" (16), "cmin" (17), "xmax" (18), "cmax" (19), "tableoid" (20), "gp_segment_id" (21)] Key sets: {[4,10]}
      |  +--CScalarCmp (>)
      |     |--CScalarIdent "a" (0)
      |     +--CScalarIdent "a" (11)
      +--CScalarIdent "a" (0)
```



# Transform

主要是将存在性和集合比较子查询转化为`COUNT(*)`计算。

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
			  GPOS_NEW(mp) CExpression(
				  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
			  GPOS_NEW(mp) CExpression(
				  mp, GPOS_NEW(mp) CPatternTree(mp))  // project list
			  )) {}
class CXformSimplifySelectWithSubquery : public CXformSimplifySubquery {
public:
	explicit CXformSimplifySelectWithSubquery(CMemoryPool *mp) :  // pattern
		  CXformSimplifySubquery(GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CLogicalSelect(mp),
			  GPOS_NEW(mp) CExpression(
				  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
			  GPOS_NEW(mp) CExpression(
				  mp, GPOS_NEW(mp) CPatternTree(mp))  // predicate tree
			  )) {} 

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
// Actual transformation to simplify subquery expression
void CXformSimplifySubquery::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
		CExpression *pexpr) const {
	CMemoryPool *mp = pxfctxt->Pmp();
	CExpression *pexprResult;
    // CUtils::FExistentialSubquery : match匹配函数, 判断是否是existential subquery
    // FSimplifyExistential: transform函数,将existential subquery转化为COUT(*)
	pexprResult = FSimplifySubquery(mp, pexpr, FSimplifyExistential, CUtils::FExistentialSubquery);
	if (nullptr != pexprResult) {
		pxfres->Add(pexprResult);
	}

    // CUtils::FQuantifiedSubquery : match匹配函数, 判断是否是quantified subquery
    // FExistentialSubquery: transform函数,将quantified subquery转化为COUT(*)
	pexprResult = FSimplifySubquery(mp, pexpr, FSimplifyQuantified, CUtils::FQuantifiedSubquery);
	if (nullptr != pexprResult) {
		pxfres->Add(pexprResult);

		// the last entry is used to replace existential subqueries with count(*)
		// after quantified subqueries have been replaced in the input expression
		pexprResult = FSimplifySubquery(mp, pexprResult, FSimplifyExistential,
										CUtils::FExistentialSubquery);
		if (nullptr != pexprResult) {
			pxfres->Add(pexprResult);
		}
	}
}

CExpression * CXformSimplifySubquery::FSimplifySubquery(CMemoryPool *mp,
          CExpression *pexprInput, FnSimplify *pfnsimplify, FnMatch *pfnmatch) {
	CExpression *pexprOuter = (*pexprInput)[0];
	CExpression *pexprScalar = (*pexprInput)[1];
	CExpression *pexprNewScalar = nullptr;

	if (!FSimplifySubqueryRecursive(mp, pexprScalar, &pexprNewScalar,
									pfnsimplify, pfnmatch)) {
		CRefCount::SafeRelease(pexprNewScalar);
		return nullptr;
	}

	pexprOuter->AddRef();
	CExpression *pexprResult = nullptr;
	if (COperator::EopLogicalSelect == pexprInput->Pop()->Eopid()) {
		pexprResult =
			CUtils::PexprLogicalSelect(mp, pexprOuter, pexprNewScalar);
	} else {
		pexprResult = CUtils::PexprLogicalProject(
			mp, pexprOuter, pexprNewScalar, false /*fNewComputedCol*/);
	}

	// normalize resulting expression
	CExpression *pexprNormalized = CNormalizer::PexprNormalize(mp, pexprResult);
	pexprResult->Release();

	return pexprNormalized;
}

BOOL CXformSimplifySubquery::FSimplifySubqueryRecursive(
	CMemoryPool *mp, CExpression *pexprScalar, CExpression **ppexprNewScalar,
	FnSimplify *pfnsimplify, FnMatch *pfnmatch) {
	// 1. 如果匹配(match)成功调用simplify函数
	if (pfnmatch(pexprScalar->Pop())) {
		return pfnsimplify(mp, pexprScalar, ppexprNewScalar);
	}

    // 2. 对于其他类型的子查询或者没有子查询,对scalar expression增加引用并返回
	if (CUtils::FSubquery(pexprScalar->Pop()) || !pexprScalar->DeriveHasSubquery()) {
		pexprScalar->AddRef();
		*ppexprNewScalar = pexprScalar;
		return true;
	}

	// 3. 递归地处理scalar的孩子
	const ULONG arity = pexprScalar->Arity();
	CExpressionArray *pdrgpexprChildren = GPOS_NEW(mp) CExpressionArray(mp);
	BOOL fSuccess = true;
	for (ULONG ul = 0; fSuccess && ul < arity; ul++) {
		CExpression *pexprChild = nullptr;
		fSuccess = FSimplifySubqueryRecursive(
			mp, (*pexprScalar)[ul], &pexprChild, pfnsimplify, pfnmatch);
		if (fSuccess) {
			pdrgpexprChildren->Append(pexprChild);
		} else {
			CRefCount::SafeRelease(pexprChild);
		}
	}

	if (fSuccess) {
		COperator *pop = pexprScalar->Pop();
		pop->AddRef();
		*ppexprNewScalar = GPOS_NEW(mp) CExpression(mp, pop, pdrgpexprChildren);
	} else {
		pdrgpexprChildren->Release();
	}

	return fSuccess;
}
```



```C++
// 适用于只含一个存在性子查询
// 将存在性子查询转化成COUNT(*),将存在性检测转化为AGG计算,可以用agg规则进行优化
// 对于Exist生成Count(*)>0, 对于Not Exist生成Count(*)=0
BOOL CXformSimplifySubquery::FSimplifyExistential(CMemoryPool *mp,
         CExpression *pexprScalar, CExpression **ppexprNewScalar) {
	CExpression *pexprNewSubquery = nullptr;
	CExpression *pexprCmp = nullptr;
    // 为existential subquery生成COUT(*) expression
	CXformUtils::ExistentialToAgg(mp, pexprScalar, &pexprNewSubquery, &pexprCmp);

	// create a comparison predicate involving subquery expression
	CExpressionArray *pdrgpexpr = GPOS_NEW(mp) CExpressionArray(mp);
	(*pexprCmp)[1]->AddRef();
	pdrgpexpr->Append(pexprNewSubquery);
	pdrgpexpr->Append((*pexprCmp)[1]);
	pexprCmp->Pop()->AddRef();

	*ppexprNewScalar = GPOS_NEW(mp) CExpression(mp, pexprCmp->Pop(), pdrgpexpr);
	pexprCmp->Release();

	return true;
}
```

```C++
/**
*	将existential subqueries改写成COUT(*)
*		exists(select * from r where a = 10) to
*			New Subquery: (select count(*) as cc from r where a = 10)
*			New Scalar: cc > 0
*
*		not exists(select * from r where a = 10) to
*			New Subquery: (select count(*) as cc from r where a = 10)
*			New Scalar: cc = 0
*/
void CXformUtils::ExistentialToAgg(CMemoryPool *mp, CExpression *pexprSubquery,
	CExpression **ppexprNewSubquery,  // output argument for new scalar subquery
	CExpression **ppexprNewScalar  // output argument for new scalar expression
) {
	COperator::EOperatorId op_id = pexprSubquery->Pop()->Eopid();
	CExpression *pexprInner = (*pexprSubquery)[0];
	IMDType::ECmpType ecmptype = IMDType::EcmptG; // >
	if (COperator::EopScalarSubqueryNotExists == op_id) {
		ecmptype = IMDType::EcmptEq; // =
	}

	pexprInner->AddRef();
	CExpression *pexprInnerNew = CUtils::PexprCountStar(mp, pexprInner);
	const CColRef *pcrCount =
		CScalarProjectElement::PopConvert((*(*pexprInnerNew)[1])[0]->Pop())
			->Pcr();

	*ppexprNewSubquery = GPOS_NEW(mp) CExpression(
		mp, GPOS_NEW(mp) CScalarSubquery(mp, pcrCount, true /*fGeneratedByExist*/,
             false /*fGeneratedByQuantified*/),pexprInnerNew);
    // 生成ScalarCmp算子(对于Exist生成pcrCount > 0, 对于Not Exist生成pcrCount=0)
	*ppexprNewScalar = CUtils::PexprCmpWithZero(mp, CUtils::PexprScalarIdent(mp, pcrCount),
								 pcrCount->RetrieveType()->MDId(), ecmptype);
}

```



```C++
//		Transform quantified subqueries to count(*) subqueries;
//		the function returns true if transformation succeeded
BOOL CXformSimplifySubquery::FSimplifyQuantified(CMemoryPool *mp,
        CExpression *pexprScalar, CExpression **ppexprNewScalar) {
	CExpression *pexprNewSubquery = nullptr;
	CExpression *pexprCmp = nullptr;
	CXformUtils::QuantifiedToAgg(mp, pexprScalar, &pexprNewSubquery, &pexprCmp);

	// create a comparison predicate involving subquery expression
	CExpressionArray *pdrgpexpr = GPOS_NEW(mp) CExpressionArray(mp);
	(*pexprCmp)[1]->AddRef();
	pdrgpexpr->Append(pexprNewSubquery);
	pdrgpexpr->Append((*pexprCmp)[1]);
	pexprCmp->Pop()->AddRef();

	*ppexprNewScalar = GPOS_NEW(mp) CExpression(mp, pexprCmp->Pop(), pdrgpexpr);
	pexprCmp->Release();
 
	return true;
}

// 将quantified subquery转化成等价的Agg表达式
void CXformUtils::QuantifiedToAgg(CMemoryPool *mp, CExpression *pexprSubquery,
	CExpression **ppexprNewSubquery,  // output argument for new scalar subquery
	CExpression **ppexprNewScalar  // output argument for new scalar expression
) {
	if (COperator::EopScalarSubqueryAll == pexprSubquery->Pop()->Eopid()) {
		return SubqueryAllToAgg(mp, pexprSubquery, ppexprNewSubquery,
								ppexprNewScalar);
	}

	return SubqueryAnyToAgg(mp, pexprSubquery, ppexprNewSubquery,
							ppexprNewScalar);
}

//		Helper for transforming SubqueryAll into aggregate subquery,
//		we generate aggregate expressions that compute the following values:
//		- N: number of null values returned by evaluating inner expression
//		- S: number of inner values matching outer value
//		the generated subquery returns a Boolean result generated by the following
//		nested-if statement:
//
//			if (inner is empty)
//				return true
//			else if (N > 0)
//				return null
//			else if (outer value is null)
//				return null
//			else if (S == 0)
//				return true
//			else
//				return false
void CXformUtils::SubqueryAllToAgg(CMemoryPool *mp, CExpression *pexprSubquery,
	CExpression **ppexprNewSubquery,  // output argument for new scalar subquery
	CExpression **ppexprNewScalar  // output argument for new scalar expression
)
    

```

