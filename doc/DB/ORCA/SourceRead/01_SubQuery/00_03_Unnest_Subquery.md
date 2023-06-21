```C++
/**
*	transform的具体实现统一由CXformSubqueryUnnest::Transform实现
*		CXformProject2Apply:
*		CXformSelect2Apply:	
*		CXformGbAgg2Apply:	
*		CXformSequenceProject2Apply
*/
class CXformSubqueryUnnest : public CXformExploration;
CXformProject2Apply::CXformProject2Apply(CMemoryPool *mp) :  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalProject(mp),
		  GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CPatternTree(mp))  // scalar project list
		  )) {}
CXformSelect2Apply::CXformSelect2Apply(CMemoryPool *mp) :  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalSelect(mp),
		  GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp)
			  CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // predicate tree
		  )){}

CXformGbAgg2Apply::CXformGbAgg2Apply(CMemoryPool *mp) :  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalGbAgg(mp),
		  GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp)
			  CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // project list
		  )) {}

CXformSequenceProject2Apply::CXformSequenceProject2Apply(CMemoryPool *mp) :  // pattern
	  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalSequenceProject(mp),
		  GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp)
			  CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // project list
		  )) {}
```

