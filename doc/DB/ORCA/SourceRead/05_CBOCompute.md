```plantuml
class CStatisticsConfig {
	- CDouble m_damping_factor_filter;
	- CDouble m_damping_factor_join;
	- CDouble m_damping_factor_groupby;
	- ULONG m_max_stats_buckets;
	- MdidHashSet *m_phsmdidcolinfo;
}
COptimizerConfig *-- ICostModel
COptimizerConfig *-- CStatisticsConfig

```



```C++
class COptimizerConfig : public CRefCount {
private:
	// plan enumeration configuration
	CEnumeratorConfig *m_enumerator_cfg;

	// 统计信息配置
	CStatisticsConfig *m_stats_conf;
	// 代价模型
	ICostModel *m_cost_model;
    
	// CTE configuration
	CCTEConfig *m_cte_conf;

	// hint configuration
	CHint *m_hint;

	// default window oids
	CWindowOids *m_window_oids;
}

// 默认Optimizer配置
//  统计信息配置采用
//	代价模型采用CCostModelGPDB，参见ICostModel::PcmDefault
COptimizerConfig * COptimizerConfig::PoconfDefault(CMemoryPool *mp)
{
	return GPOS_NEW(mp) COptimizerConfig(
		GPOS_NEW(mp) CEnumeratorConfig(mp, 0 /*plan_id*/, 0 /*ullSamples*/),
		CStatisticsConfig::PstatsconfDefault(mp),
		CCTEConfig::PcteconfDefault(mp), ICostModel::PcmDefault(mp),
		CHint::PhintDefault(mp), CWindowOids::GetWindowOids(mp));
}

static CStatisticsConfig * PstatsconfDefault(CMemoryPool *mp) {
    return GPOS_NEW(mp) CStatisticsConfig(
        mp, 0.75 /* damping_factor_filter */,
        0.01 /* damping_factor_join */, 0.75 /* damping_factor_groupby */,
        MAX_STATS_BUCKETS);
}
```



# 代价模型数据结构

```plantuml
CCostModelGPDB -right--|> ICostModel
IStatistics -left--* ICostModel
CCostModelGPDB *-- CCostModelParamsGPDB
CCostModelParamsGPDB -right--|> ICostModelParams
```



```C++
// 重写了一系列的运算符,如+,=,*<>
class CCost : public CDouble;
// GPDB代价模型
class CCostModelGPDB : public ICostModel
// GPDB代价模型参数
class CCostModelParamsGPDB : public ICostModelParams;

class ICostModelParams : public CRefCount {
public:
	struct SCostParam {
	private:
		ULONG m_id; 		// 参数识别符
		CDouble m_value;	// 参数的值
		// 参数的下限和上限
		CDouble m_lower_bound_val;
		CDouble m_upper_bound_val;
	}
	// 根据param id,name查找cost param
	virtual SCostParam *PcpLookup(ULONG id) const = 0;
	virtual SCostParam *PcpLookup(const CHAR *szName) const = 0;

	// 根据param id, name设置param值
	virtual void SetParam(ULONG id, CDouble dVal, CDouble dLowerBound, CDouble dUpperBound) = 0;
	virtual void SetParam(const CHAR *szName, CDouble dVal, CDouble dLowerBound,CDouble dUpperBound) = 0;

	virtual BOOL Equals(ICostModelParams *pcm) const = 0;
	// 根据param id查找param name
	virtual const CHAR *SzNameLookup(ULONG id) const = 0;
};

class ICostModel : public CRefCount {
private:
	// stats of the root
	IStatistics *m_pstats;

public:
	// return number of hosts (nodes) that store data
	virtual ULONG UlHosts() const = 0;

	// return number of rows per host
	virtual CDouble DRowsPerHost(CDouble dRowsTotal) const = 0;

	// 获取和设置代价模型参数
	virtual ICostModelParams *GetCostModelParams() const = 0;
	void SetParams(ICostModelParamsArray *pdrgpcp) const;

	// main driver for cost computation
	virtual CCost Cost(CExpressionHandle &exprhdl, const SCostingInfo *pci) const = 0;

	// cost model type
	virtual ECostModelType Ecmt() const = 0;

	// 创建默认代价模型(CCostModelGPDB)
	static ICostModel *PcmDefault(CMemoryPool *mp) {
		return GPOS_NEW(mp) CCostModelGPDB(mp, GPOPT_DEFAULT_SEGMENT_COUNT);
	}
}
```



```C++
// Get candidate xforms
CXformSet * CLogicalNAryJoin::PxfsCandidates(CMemoryPool *mp) const {
    CXformSet *xform_set = GPOS_NEW(mp) CXformSet(mp);

    (void) xform_set->ExchangeSet(CXform::ExfSubqNAryJoin2Apply);
    (void) xform_set->ExchangeSet(CXform::ExfExpandNAryJoin);
    (void) xform_set->ExchangeSet(CXform::ExfExpandNAryJoinMinCard);
    (void) xform_set->ExchangeSet(CXform::ExfExpandNAryJoinDP);
    (void) xform_set->ExchangeSet(CXform::ExfExpandNAryJoinGreedy);
    (void) xform_set->ExchangeSet(CXform::ExfExpandNAryJoinDPv2);

    return xform_set;
}
```




```C++
// Lookup the best context across all stages for the given required propertie
// called by CJobGroupExpressionOptimization::EevtInitialize
COptimizationContext * CGroup::PocLookupBest(CMemoryPool *mp,
		ULONG ulSearchStages, CReqdPropPlan *prpp) {
	COptimizationContext *pocBest = nullptr;
	CCostContext *pccBest = nullptr;
	for (ULONG ul = 0; ul < ulSearchStages; ul++) {
		COptimizationContext *pocCurrent = PocLookup(mp, prpp, ul);
		if (nullptr == pocCurrent) {
			continue;
		}

		CCostContext *pccCurrent = pocCurrent->PccBest();
		if (nullptr == pccBest ||
			(nullptr != pccCurrent && pccCurrent->FBetterThan(pccBest))) {
			pocBest = pocCurrent;
			pccBest = pccCurrent;
		}
	}

	return pocBest;
}

//		Compute and store expression's cost under a given context;
//		the function returns the cost context containing the computed cost
CCostContext * CGroupExpression::PccComputeCost(
	CMemoryPool *mp, COptimizationContext *poc, ULONG ulOptReq,
	COptimizationContextArray *pdrgpoc,	 // array of child contexts
	BOOL fPruned,  // is created cost context pruned based on cost bound
	CCost costLowerBound	// lower bound on the cost of plan carried by cost context
) {

	if (!fPruned && !FValidContext(mp, poc, pdrgpoc))
	{
		return nullptr;
	}

	// check if the same cost context is already created for current group expression
	if (FCostContextExists(poc, pdrgpoc))
	{
		return nullptr;
	}

	poc->AddRef();
	this->AddRef();
	CCostContext *pcc = GPOS_NEW(mp) CCostContext(mp, poc, ulOptReq, this);
	BOOL fValid = true;

	// computing cost
	pcc->SetState(CCostContext::estCosting);

	if (!fPruned)
	{
		if (nullptr != pdrgpoc)
		{
			pdrgpoc->AddRef();
		}
		pcc->SetChildContexts(pdrgpoc);

		fValid = pcc->IsValid(mp);
		if (fValid)
		{
			CCost cost = CostCompute(mp, pcc);
			pcc->SetCost(cost);
		}
		GPOS_ASSERT_IMP(COptCtxt::FAllEnforcersEnabled(),
						fValid && "Cost context carries an invalid plan");
	}
	else
	{
		pcc->SetPruned();
		pcc->SetCost(costLowerBound);
	}

	pcc->SetState(CCostContext::estCosted);
	if (fValid)
	{
		return PccInsertBest(pcc);
	}

	pcc->Release();

	// invalid cost context
	return nullptr;
}

CCost CGroupExpression::CostCompute(CMemoryPool *mp, CCostContext *pcc) {

	// prepare cost array
	COptimizationContextArray *pdrgpoc = pcc->Pdrgpoc();
	CCostArray *pdrgpcostChildren = GPOS_NEW(mp) CCostArray(mp);
	const ULONG length = pdrgpoc->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		COptimizationContext *pocChild = (*pdrgpoc)[ul];
		pdrgpcostChildren->Append(GPOS_NEW(mp)
									  CCost(pocChild->PccBest()->Cost()));
	}

	CCost cost = pcc->CostCompute(mp, pdrgpcostChildren);
	pdrgpcostChildren->Release();

	return cost;
}
```



```C++
//		Compute a lower bound on plans rooted by current group expression for
//		the given required properties
// called by CJobGroupExpressionOptimization::EevtInitialize
CCost CGroupExpression::CostLowerBound(CMemoryPool *mp, CReqdPropPlan *prppInput,
								 CCostContext *pccChild, ULONG child_index) {
	prppInput->AddRef();
	if (nullptr != pccChild) {
		pccChild->AddRef();
	}
	CPartialPlan *ppp =
		GPOS_NEW(mp) CPartialPlan(this, prppInput, pccChild, child_index);
	CCost *pcostLowerBound = m_ppartialplancostmap->Find(ppp);
	if (nullptr != pcostLowerBound) {
		ppp->Release();
		return *pcostLowerBound;
	}

	// compute partial plan cost
	CCost cost = ppp->CostCompute(mp);

	BOOL fSuccess GPOS_ASSERTS_ONLY =
		m_ppartialplancostmap->Insert(ppp, GPOS_NEW(mp) CCost(cost.Get()));
	GPOS_ASSERT(fSuccess);
	return cost;
}
```



```C++
void
CGroup::UpdateBestCost(COptimizationContext *poc, CCostContext *pcc)
{
	GPOS_ASSERT(CCostContext::estCosted == pcc->Est());

	COptimizationContext *pocFound = nullptr;

	{
		// scope for accessor
		ShtAcc shta(Sht(), *poc);
		pocFound = shta.Find();
	}

	if (nullptr == pocFound)
	{
		// it should never happen, but instead of crashing, raise an exception
		GPOS_RAISE(CException::ExmaInvalid, CException::ExmiInvalid,
				   GPOS_WSZ_LIT(
					   "Updating cost for non-existing optimization context"));
	}

	// update best cost context
	CCostContext *pccBest = pocFound->PccBest();
	if (GPOPT_INVALID_COST != pcc->Cost() &&
		(nullptr == pccBest || pcc->FBetterThan(pccBest)))
	{
		pocFound->SetBest(pcc);
	}
}
```

