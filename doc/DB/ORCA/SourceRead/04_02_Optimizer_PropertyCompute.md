在`CJobGroupExpressionOptimization`中计算当前孩子的`required plan`属性并derive前一个孩子的`plan property`

```
EevtOptimizeChildren(状态机调用入口)
	-> ScheduleChildGroupsJobs
		-> ComputeCurrentChildRequirements(负责当前孩子的required plan属性计算，主逻辑在此)
```

```C++
void CJobGroupExpressionOptimization::ComputeCurrentChildRequirements(
    		CSchedulerContext *psc) {
	if (m_ulChildIndex != m_pexprhdlPlan->UlFirstOptimizedChildIndex()) {
        // derive前一个child的plan property(重要:derive plan prop的入口函数)
		DerivePrevChildProps(psc);
		if (m_fChildOptimizationFailed) {
			return;
		}
	}

	if (0 == m_ulChildIndex && nullptr != m_prppCTEProducer) {
		m_prppCTEProducer->AddRef();
		m_pexprhdlPlan->CopyChildReqdProps(m_ulChildIndex, m_prppCTEProducer);
	} else {
        // 计算第n个孩子的reqired plan prop(重要: 计算required plan prop的入口函数)
        // 实现在CExpressionHandle::ComputeChildReqdProps
		m_pexprhdlPlan->ComputeChildReqdProps(m_ulChildIndex, m_pdrgpdp, m_ulOptReq);
	}
}
```

# Derive Prop

```C++
//		Derive plan properties and stats of the child previous to
//		the one being optimized
void CJobGroupExpressionOptimization::DerivePrevChildProps(CSchedulerContext *psc) {
	ULONG ulPrevChildIndex =
		m_pexprhdlPlan->UlPreviousOptimizedChildIndex(m_ulChildIndex);

	// retrieve plan properties of the optimal implementation of previous child group
	CGroup *pgroupChild = (*m_pgexpr)[ulPrevChildIndex];
	if (pgroupChild->FScalar()) {
		return;
	}

	COptimizationContext *pocChild = pgroupChild->PocLookupBest(
		psc->GetGlobalMemoryPool(), psc->Peng()->UlSearchStages(),
		m_pexprhdlPlan->Prpp(ulPrevChildIndex));

	CCostContext *pccChildBest = pocChild->PccBest();
	if (nullptr == pccChildBest) { // failed to optimize child
		m_fChildOptimizationFailed = true;
		return;
	}

	// check if job can be early terminated after previous children have been optimized
	CCost costLowerBound(GPOPT_INVALID_COST);
	if (psc->Peng()->FSafeToPrune(m_pgexpr, m_poc->Prpp(), pccChildBest,
								  ulPrevChildIndex, &costLowerBound)) {
		// failed to optimize child due to cost bounding
		(void) m_pgexpr->PccComputeCost(psc->GetGlobalMemoryPool(), m_poc,
										m_ulOptReq, nullptr /*pdrgpoc*/,
										true /*fPruned*/, costLowerBound);
		m_fChildOptimizationFailed = true;
		return;
	}

	CExpressionHandle exprhdl(psc->GetGlobalMemoryPool());
	exprhdl.Attach(pccChildBest);
	exprhdl.DerivePlanPropsForCostContext();
	exprhdl.Pdp()->AddRef();
	m_pdrgpdp->Append(exprhdl.Pdp());

	// copy stats of child's best cost context to current stats context
	IStatistics *pstat = pccChildBest->Pstats();
	pstat->AddRef();
	m_pdrgpstatCurrentCtxt->Append(pstat);
}
```



# 计算Required Plan Prop

```C++
// 计算第N个孩子的required enforece属性
void CExpressionHandle::ComputeChildReqdProps(ULONG child_index,
		CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) {
    // 在CJobGroupExpressionOptimization::InitChildGroupsOptimization中
    // 用COptimizationContext中的Prpp()调用InitReqdProps
    // 得到一个local required plan property
	CReqdProp *prp = m_prp;
	if (FScalarChild(child_index)) {
		// use local reqd properties to fill scalar child entry in children array
		prp->AddRef();
	} else {
        // 对于物理算子required plan属性对应于CReqdPropPlan
		prp = Pop()->PrpCreate(m_mp);
        // 通过CReqdPropPlan::Compute计算required plan属性
		prp->Compute(m_mp, *this, m_prp, child_index, pdrgpdpCtxt, ulOptReq);
	}
	// 替换对应孩子的required属性
	m_pdrgprp->Replace(child_index, prp);
}

//	Compute required props
void CReqdPropPlan::Compute(CMemoryPool *mp, CExpressionHandle &exprhdl,
					   CReqdProp *prpInput, ULONG child_index,
					   CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) {
	CReqdPropPlan *prppInput = CReqdPropPlan::Prpp(prpInput);
	CPhysical *popPhysical = CPhysical::PopConvert(exprhdl.Pop());
	ComputeReqdCols(mp, exprhdl, prpInput, child_index, pdrgpdpCtxt);
	ComputeReqdCTEs(mp, exprhdl, prpInput, child_index, pdrgpdpCtxt);

	ULONG ulOrderReq = 0;
	ULONG ulDistrReq = 0;
	ULONG ulRewindReq = 0;
	ULONG ulPartPropagateReq = 0;
	popPhysical->LookupRequest(ulOptReq, &ulOrderReq, &ulDistrReq, &ulRewindReq,
							   &ulPartPropagateReq);
	// 1. 生成enforece order属性
    // 		1.1. 对应物理算子实现Physical::PosRequired接口来计算第n个孩子required order属性
    // 		1.2. Physical::Eom计算order matching type？
    // 参见CPhysical::Ped实现
	m_peo = GPOS_NEW(mp) CEnfdOrder(
		popPhysical->PosRequired(mp, exprhdl, prppInput->Peo()->PosRequired(),
								 child_index, pdrgpdpCtxt, ulOrderReq),
		popPhysical->Eom(prppInput, child_index, pdrgpdpCtxt, ulOrderReq));

    // 2. 生成enforce分布式属性,通过CPhysical::Ped
    //		2.1 对应物理算子实现Physical::PdsRequired计算第n个孩子的分布式属性
    //		2.2 Physical::Edm计算分布式matching function
	m_ped = popPhysical->Ped(mp, exprhdl, prppInput, child_index, pdrgpdpCtxt,
							 ulDistrReq);

	m_per = GPOS_NEW(mp) CEnfdRewindability(
		popPhysical->PrsRequired(mp, exprhdl, prppInput->Per()->PrsRequired(),
								 child_index, pdrgpdpCtxt, ulRewindReq),
		popPhysical->Erm(prppInput, child_index, pdrgpdpCtxt, ulRewindReq));

	m_pepp = GPOS_NEW(mp) CEnfdPartitionPropagation(
		popPhysical->PppsRequired(mp, exprhdl,
								  prppInput->Pepp()->PppsRequired(),
								  child_index, pdrgpdpCtxt, ulPartPropagateReq),
		CEnfdPartitionPropagation::EppmSatisfy);
}

//-------------------------------------------------------------------------------------
// Required Plan Properties
//-------------------------------------------------------------------------------------

// 生成第n个孩子的enforce分布式属性，默认实现，虚函数，派生类可以重写
CEnfdDistribution *CPhysical::Ped(CMemoryPool *mp, CExpressionHandle &exprhdl,
			   CReqdPropPlan *prppInput, ULONG child_index,
			   CDrvdPropArray *pdrgpdpCtxt, ULONG ulDistrReq) {
	return GPOS_NEW(mp) CEnfdDistribution(
		PdsRequired(mp, exprhdl, prppInput->Ped()->PdsRequired(), child_index,
					pdrgpdpCtxt, ulDistrReq),
		Edm(prppInput, child_index, pdrgpdpCtxt, ulDistrReq));
}

// 计算第n个孩子需要的分布式属性
// 纯虚接口,每个具体的物理算子进行具体的实现
CDistributionSpec * CPhysical::PdsRequired(CMemoryPool *mp,
		CExpressionHandle &exprhdl,
		CDistributionSpec *pdsRequired, ULONG child_index,
    	CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) const = 0;
```



```C++
// 计算第N个孩子required列
void CExpressionHandle::ComputeChildReqdCols(ULONG child_index, CDrvdPropArray *pdrgpdpCtxt) {
	CReqdProp *prp = m_prp;
	if (FScalarChild(child_index)) {
		// use local reqd properties to fill scalar child entry in children array
		prp->AddRef();
	} else {
		// compute required columns
		prp = Pop()->PrpCreate(m_mp);
		CReqdPropPlan::Prpp(prp)->ComputeReqdCols(m_mp, *this, m_prp, child_index, pdrgpdpCtxt);
	}
	// replace required properties of given child
	m_pdrgprp->Replace(child_index, prp);
}

void CReqdPropPlan::ComputeReqdCols(CMemoryPool *mp, CExpressionHandle &exprhdl,
							   CReqdProp *prpInput, ULONG child_index,
							   CDrvdPropArray *pdrgpdpCtxt) {
	CReqdPropPlan *prppInput = CReqdPropPlan::Prpp(prpInput);
	CPhysical *popPhysical = CPhysical::PopConvert(exprhdl.Pop());
	m_pcrs = popPhysical->PcrsRequired(mp, exprhdl, prppInput->PcrsRequired(),
                                       child_index, pdrgpdpCtxt, 0 /*ulOptReq*/);
}
```



# 添加Required Enforcer

```C++
// Add required enforcers to owning group
CJobGroupExpressionOptimization::EEvent
CJobGroupExpressionOptimization::EevtAddEnforcers(CSchedulerContext *psc, CJob *pjOwner) {
	CJobGroupExpressionOptimization *pjgeo = PjConvert(pjOwner);

	// build child contexts array
	pjgeo->m_pdrgpoc = psc->Peng()->PdrgpocChildren(psc->GetGlobalMemoryPool(),
													*pjgeo->m_pexprhdlPlan);

	// enforce物理属性,参见CEngine::FCheckEnfdProps
	BOOL fCheckEnfdProps = psc->Peng()->FCheckEnfdProps(
		psc->GetGlobalMemoryPool(), pjgeo->m_pgexpr, pjgeo->m_poc,
		pjgeo->m_ulOptReq, pjgeo->m_pdrgpoc);
	if (fCheckEnfdProps) {
		// No new enforcers group expressions were added because they were either
		// optional or unnecessary. So, move on to optimize the current group expression.
		return eevOptimizingSelf;
	}

	// Either adding enforcers was prohibited or at least one enforcer was added
	// because it was required. In any case, this job can be finalized, since
	// optimizing the current group expression is not needed (because of the
	// prohibition) or the newly created enforcer group expression job will get
	// to it later on.
	pjgeo->Cleanup();
	return eevFinalized;
}
```

