算子采用`COperator`基类，在其上可以派生出

```C++
// 逻辑算子
class CLogical : public COperator;
// 物理算子
class CPhysical : public COperator;
class CPattern : public COperator;
class CScalar : public COperator;
```

先了解`COperator`算子基类的表示

```C++
class COperator : public CRefCount, public DbgPrintMixin<COperator> {
protected:
	// is pattern of xform
	BOOL m_fPattern;
 
    // 对于当前查询, 所有的算子实例中唯一的Operator ID
	ULONG m_ulOpId;
	// 生成唯一的Operator ID, 在每个Operator的构造中加1将其值复制给m_ulOpId
	static ULONG m_aulOpIdCounter;
public:
    // 算子的标识和名称
    virtual EOperatorId Eopid() const = 0;
	virtual const CHAR *SzId() const = 0;
    
    // 算子唯一operator id,通过m_aulOpIdCounter在Operator构造时自动加一
    ULONG UlOpId() const { return m_ulOpId; }
    
    // 可用于标识算子的类型,是logical,physical,scalar或pattern
	virtual BOOL FLogical() const { return false; }
	virtual BOOL FPhysical() const { return false; }
	virtual BOOL FScalar() const { return false; }
    virtual BOOL FPattern() const { return false; }
    
	virtual ULONG HashValue() const;

	// sensitivity to order of inputs
	virtual BOOL FInputOrderSensitive() const = 0;

	// match function;
	// abstract to enforce an implementation for each new operator
	virtual BOOL Matches(COperator *pop) const = 0;

	// 为derived属性和required属性创建相应的属性类,对外提供统一的接口
    //      1. derived属性的基类是CDrvdProp
    // 		2. required属性的基类是CReqdProp
    // 在CScalar, CLogical, Cphysical基类中实现这两个接口
	virtual CDrvdProp *PdpCreate(CMemoryPool *mp) const = 0;
	virtual CReqdProp *PrpCreate(CMemoryPool *mp) const = 0;

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist) = 0;
};
```

|     算子     |      derived属性      |      require属性      |
| :----------: | :-------------------: | :-------------------: |
| Logical算子  | `CDrvdPropRelational` | `CReqdPropRelational` |
| Physical算子 |    `CDrvdPropPlan`    |    `CReqdPropPlan`    |
|  Scalar算子  |   `CDrvdPropScalar`   |   `CDrvdPropScalar`   |
| Pattern算子  |          无           |          无           |

# LogicalOperator

```C++
class CLogical : public COperator {
public:
	//-------------------------------------------------------------------------------------
	// Derived Relational Properties
	//-------------------------------------------------------------------------------------
	virtual CColRefSet *DeriveOutputColumns(CMemoryPool *mp, CExpressionHandle &exprhdl) = 0;

	// derive outer references
    // outer references是scalar孩子使用的列，但不含在relation孩子的output columns
	virtual CColRefSet * DeriveOuterReferences(CMemoryPool *mp, CExpressionHandle &exprhdl)

	// derive outer references for index get and dynamic index get operators
	virtual CColRefSet *PcrsDeriveOuterIndexGet(CMemoryPool *mp,CExpressionHandle &exprhdl);

	// derive not nullable output columns
	virtual CColRefSet *DeriveNotNullColumns(CMemoryPool *mp,CExpressionHandle &) const;

	// derive columns from the inner child of a correlated-apply expression that can be used above the apply expression
	virtual CColRefSet *DeriveCorrelatedApplyColumns(CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	// derive key collections
	virtual CKeyCollection *DeriveKeyCollection(CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	// derive max card
	virtual CMaxCard DeriveMaxCard(CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	// derive join depth
	virtual ULONG DeriveJoinDepth(CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	// derive partition information
	virtual CPartInfo *DerivePartitionInfo(CMemoryPool *mp, CExpressionHandle &exprhdl) const = 0;

	// derive约束条件属性,数据模型中存在约束条件
    // CLogical提供了CLogical::PpcDeriveConstraintPassThru
    // 实现获取指定孩子的约束条件
	virtual CPropConstraint *DerivePropertyConstraint(CMemoryPool *mp,
		CExpressionHandle &exprhdl) const = 0;

	// derive function properties
	virtual CFunctionProp *DeriveFunctionProperties(CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	virtual CTableDescriptor *DeriveTableDescriptor(CMemoryPool *mp, CExpressionHandle &exprhdl) const;
    
    //-------------------------------------------------------------------------------------
	// Derived Stats
	//-------------------------------------------------------------------------------------
    // 当Group以及其child group完成exploration,开始derive statistics
    // 参见CJobGroupExploration::EevtExploreChildren
    // orca会选择group中选择promise高group expression进行stat derivation
    // 参见(CGroup::PstatsRecursiveDerive -> CGroup::PgexprBestPromise)
	virtual IStatistics *PstatsDerive(CMemoryPool *mp,
              CExpressionHandle &exprhdl, IStatisticsArray *stats_ctxt) const = 0;
	// 计算stat promise
	virtual EStatPromise Esp(CExpressionHandle &) const = 0
        
   	//-------------------------------------------------------------------------------------
	// Required Relational Properties
	//-------------------------------------------------------------------------------------
	// 为required属性创建相应的属性类,对外提供统一的接口
    // logical算子的required属性对应参见CReqdPropRelational并通过compute计算stat columns
	CReqdProp *PrpCreate(CMemoryPool *mp) const override;
	// 计算第n个孩子required stat columns
    // 参见CReqdPropRelational::Compute
	virtual CColRefSet *PcrsStat(CMemoryPool *mp, CExpressionHandle &exprhdl,
             CColRefSet *pcrsInput, ULONG child_index) const = 0;
    
    //-------------------------------------------------------------------------------------
	// Transformations
	//-------------------------------------------------------------------------------------
    // 候选的xform id的集合,算子实现它可能进行的xform transformation变换
    // 在Job Transformation阶段获取到该集合并和xform factory支持的xform
    // 以及search stage设置的取交集而进行transform(参见	
    // CJobGroupExpressionExploration::ScheduleApplicableTransformations
 	// CJobGroupExpressionImplementation::ScheduleApplicableTransformations)
    // orca实现了一个CBitSet将枚举映射为bit位信息,并在其之上构造了一些列的集合操作
    // CXformSet -> CEnumSet(继承自CBitSet)
    virtual CXformSet *PxfsCandidates(CMemoryPool *mp) const = 0;
}
```
## 关系属性

derive约束条件

```C++
// derive约束条件属性,数据模型中存在约束条件
// CLogical定义了纯虚函数派生子类需要自定义实现
virtual CPropConstraint *DerivePropertyConstraint(CMemoryPool *mp,
    CExpressionHandle &exprhdl) const = 0;

// CLogical提供了默认实现实现获取指定孩子的约束条件
// 在其上派生的子类, 可以通过该接口实现derive指定孩子的约束条件
// 例如,窗口CLogicalSequenceProject::DerivePropertyConstraint
//				-> PpcDeriveConstraintPassThru(exprhdl, 0);
CPropConstraint * CLogical::PpcDeriveConstraintPassThru(
    	CExpressionHandle &exprhdl, ULONG ulChild){
	// return constraint property of child
	CPropConstraint *ppc = exprhdl.DerivePropertyConstraint(ulChild);
	if (NULL != ppc) {
		ppc->AddRef();
	}
	return ppc;
}
```

`CLogical`算子提供了一些公共函数实现`derive`约束条件。

```C++
// derive constraint property from a table/index get
static CPropConstraint *PpcDeriveConstraintFromTable(
    CMemoryPool *mp, const CTableDescriptor *ptabdesc,
    const CColRefArray *pdrgpcrOutput);

// derive constraint property from a table/index get with predicates
static CPropConstraint *PpcDeriveConstraintFromTableWithPredicates(
    CMemoryPool *mp, CExpressionHandle &exprhdl,
    const CTableDescriptor *ptabdesc, const CColRefArray *pdrgpcrOutput);

// shorthand to addref and pass through constraint from a given child
static CPropConstraint *PpcDeriveConstraintPassThru(
    CExpressionHandle &exprhdl, ULONG ulChild);

// derive constraint property only on the given columns
static CPropConstraint *PpcDeriveConstraintRestrict(
    CMemoryPool *mp, CExpressionHandle &exprhdl, CColRefSet *pcrsOutput);
```



derive outer reference

```C++
// Derive outer references
CColRefSet* CLogical::DeriveOuterReferences(CMemoryPool *mp,
		CExpressionHandle &exprhdl, CColRefSet *pcrsUsedAdditional) {
	ULONG arity = exprhdl.Arity();
	CColRefSet *outer_refs = GPOS_NEW(mp) CColRefSet(mp);
	
    // collect output columns from relational children
	// and used columns from scalar children
	CColRefSet *pcrsOutput = GPOS_NEW(mp) CColRefSet(mp);
	CColRefSet *pcrsUsed = GPOS_NEW(mp) CColRefSet(mp);
	for (ULONG i = 0; i < arity; i++) {
		if (exprhdl.FScalarChild(i)) {
			pcrsUsed->Union(exprhdl.DeriveUsedColumns(i));
		} else {
			// add outer references from relational children
			outer_refs->Union(exprhdl.DeriveOuterReferences(i));
			pcrsOutput->Union(exprhdl.DeriveOutputColumns(i));
		}
	}

	if (NULL != pcrsUsedAdditional) {
		pcrsUsed->Include(pcrsUsedAdditional);
	}

	// outer references are columns used by scalar child
	// but are not included in the output columns of relational children
	outer_refs->Union(pcrsUsed);
	outer_refs->Exclude(pcrsOutput);

	pcrsOutput->Release();
	pcrsUsed->Release();
	return outer_refs;
}
```



## Project

```C++
// Project operator
class CLogicalProject : public CLogicalUnary
```
## Agg


```C++
​```C++
class CLogicalGbAgg : public CLogicalUnary {
protected:
	// does local / intermediate / global aggregate generate duplicate values for the same group
	BOOL m_fGeneratesDuplicates;

	// array of columns used in distinct qualified aggregates (DQA)
	// used only in the case of intermediate aggregates
	CColRefArray *m_pdrgpcrArgDQA;

	// compute required stats columns for a GbAgg
	CColRefSet *PcrsStatGbAgg(CMemoryPool *mp, CExpressionHandle &exprhdl,
							  CColRefSet *pcrsInput, ULONG child_index,
							  CColRefArray *pdrgpcrGrp) const;
            ....

private:
	// array of grouping columns
	CColRefArray *m_pdrgpcr;

	// minimal grouping columns based on FD's
	CColRefArray *m_pdrgpcrMinimal;

	// local / intermediate / global aggregate
	COperator::EGbAggType m_egbaggtype;

	// which type of multi-stage agg it is
	EAggStage m_aggStage;

};	// class CLogicalGbAgg
```
```
# PhysicalOperator

​```C++
class CPhysical : public COperator {
public:
	//-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------
	// compute required output columns of the n-th child
	virtual CColRefSet *PcrsRequired(
		CMemoryPool *mp, CExpressionHandle &exprhdl, CColRefSet *pcrsRequired,
		ULONG child_index, CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) = 0;

	// compute required ctes of the n-th child
	virtual CCTEReq *PcteRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
              CCTEReq *pcter, ULONG child_index, CDrvdPropArray *pdrgpdpCtxt,
              ULONG ulOptReq) const = 0;

	// compute distribution spec from the table descriptor
	static CDistributionSpec *PdsCompute(CMemoryPool *mp,
             const CTableDescriptor *ptabdesc, CColRefArray *pdrgpcrOutput,
             CColRef *gp_segment_id);

	// compute required sort order of the n-th child
	virtual COrderSpec *PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
                COrderSpec *posRequired, ULONG child_index,
                CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) const = 0;

	// compute required rewindability of the n-th child
	virtual CRewindabilitySpec *PrsRequired(CMemoryPool *mp,
                CExpressionHandle &exprhdl, CRewindabilitySpec *prsRequired,
                ULONG child_index, CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) const = 0;

	// compute required partition propoagation spec of the n-th child
	virtual CPartitionPropagationSpec *PppsRequired(
		CMemoryPool *mp, CExpressionHandle &exprhdl,
		CPartitionPropagationSpec *pppsRequired, ULONG child_index,
		CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) const;


	// required properties: check if required columns are included in output columns
	virtual BOOL FProvidesReqdCols(CExpressionHandle &exprhdl,
               CColRefSet *pcrsRequired, ULONG ulOptReq) const = 0;

	// required properties: check if required CTEs are included in derived CTE map
	virtual BOOL FProvidesReqdCTEs(CExpressionHandle &exprhdl,const CCTEReq *pcter) const;
    
    //-------------------------------------------------------------------------------------
	// Derived Plan Properties
	//-------------------------------------------------------------------------------------
	// derive sort order
	virtual COrderSpec *PosDerive(CMemoryPool *mp,CExpressionHandle &exprhdl) const = 0;
	// derive distribution
	virtual CDistributionSpec *PdsDerive(CMemoryPool *mp,
             CExpressionHandle &exprhdl) const = 0;

	// derived properties: derive rewindability
	virtual CRewindabilitySpec *PrsDerive(CMemoryPool *mp,
                                          CExpressionHandle &exprhdl) const = 0;
	// derived properties: derive partition propagation spec
	virtual CPartitionPropagationSpec *PppsDerive(
			CMemoryPool *mp, CExpressionHandle &exprhdl) const;
	// derive cte map
	virtual CCTEMap *PcmDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const;
    
    //-------------------------------------------------------------------------------------
	// Enforced Properties
	// See CEngine::FCheckEnfdProps() for comments on usage.
	//-------------------------------------------------------------------------------------

	// return order property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetOrder(
		CExpressionHandle &exprhdl, const CEnfdOrder *peo) const = 0;

	// return distribution property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetDistribution(
		CExpressionHandle &exprhdl, const CEnfdDistribution *ped) const;

	// return rewindability property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetRewindability(
		CExpressionHandle &exprhdl, const CEnfdRewindability *per) const = 0;

	// return partition propagation property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetPartitionPropagation(
		CExpressionHandle &exprhdl, const CEnfdPartitionPropagation *per) const;

	// order matching type
	virtual CEnfdOrder::EOrderMatching Eom(CReqdPropPlan *prppInput,
           ULONG child_index, CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq);

	// rewindability matching type
	virtual CEnfdRewindability::ERewindabilityMatching Erm(
		CReqdPropPlan *prppInput, ULONG child_index,
		CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq);
};
```

```C++
// Helper, 用于计算child_index孩子的required output columns
// 需要调用方的ulScalarIndex-th孩子是scalar
CColRefSet * CPhysical::PcrsChildReqd(CMemoryPool *mp, CExpressionHandle &exprhdl,
     CColRefSet *pcrsRequired, ULONG child_index, ULONG ulScalarIndex) {
    pcrsRequired->AddRef();
    // 通过Hash Map(m_phmrcr)缓存了child的columns request
    // 构造CReqdColsRequest用于m_phmrcr的查找
    // HashValue是根据child index计算
	CReqdColsRequest *prcr =
        	GPOS_NEW(mp) CReqdColsRequest(pcrsRequired, child_index, ulScalarIndex);
	CColRefSet *pcrs = NULL;
	pcrs = m_phmrcr->Find(prcr);
    // 在缓存的child columns request map中查找成功
    // 将对应的n-th孩子的required columns直接返回,不用计算
	if (NULL != pcrs) {
		prcr->Release();
		pcrs->AddRef();
		return pcrs;
	}

	// 缓存没命中，需要计算
	pcrs = GPOS_NEW(mp) CColRefSet(mp, *pcrsRequired);
	if (gpos::ulong_max != ulScalarIndex) {
		// include used columns and exclude defined columns of scalar child
		pcrs->Union(exprhdl.DeriveUsedColumns(ulScalarIndex));
		pcrs->Exclude(exprhdl.DeriveDefinedColumns(ulScalarIndex));
	}

	// 和child_index-th孩子的output columns取交集
	pcrs->Intersection(exprhdl.DeriveOutputColumns(child_index));

	// 将计算出的n-th的required columns request信息插入缓存的Hash Map
	pcrs->AddRef();
	m_phmrcr->Insert(prcr, pcrs);
	return pcrs;
}
```



# Pattern

```C++
class CPattern : public COperator {
public:
    // 标识算子是否是pattern leaf
    virtual BOOL FLeaf() const = 0;
    
    // 检查pattern是否是multi node
    // 其中, CPatternMultiLeaf和CPatternMultiTree是multi node
    static BOOL FMultiNode(COperator *pop) {
		return COperator::EopPatternMultiLeaf == pop->Eopid() ||
			   COperator::EopPatternMultiTree == pop->Eopid();
	}
}
```

```C++
class CPatternMultiTree : public CPattern;
class CPatternMultiLeaf : public CPattern;
class CPatternLeaf : public CPattern;
class CPatternNode : public CPattern;
class CPatternTree : public CPattern;
```



# Scalar

```C++
class CScalar : public COperator {
    
}
```



