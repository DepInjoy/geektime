## 物理算子

```C++
class CPhysicalSequence : public CPhysical {
private:
    // empty column set, 构造时创建
	CColRefSet *m_pcrsEmpty; 
}
```



 ```C++
//---------------------------------------------------------------------
// Required Plan Properties
//---------------------------------------------------------------------

// 计算n-th孩子的required output columns
virtual CColRefSet *PcrsRequired(CMemoryPool *mp,
         CExpressionHandle &exprhdl,CColRefSet *pcrsRequired,
         ULONG child_index, CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) {
    const ULONG arity = exprhdl.Arity();
    // 窗口应该有两个孩子:
    // 		relation child(arity - 1)
    //		scalarWindow child(arity)
    // 如果是relation child计算required output columns
    if (child_index == arity - 1) {
        // 利用CPhysical提供的计算计算n-th孩子的required columns进行计算
        return PcrsChildReqd(mp, exprhdl, pcrsRequired, child_index, gpos::ulong_max);
    }

    // 否则,是scalar的required output columns计算,返回空
    m_pcrsEmpty->AddRef();
    return m_pcrsEmpty; // 在构造函数创建
}

	// compute required ctes of the n-th child
	virtual CCTEReq *PcteRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
								  CCTEReq *pcter, ULONG child_index,
								  CDrvdPropArray *pdrgpdpCtxt,
								  ULONG ulOptReq) const;

// compute required sort columns of the n-th child
virtual COrderSpec *PosRequired(CMemoryPool *,CExpressionHandle &exprhdl,
        COrderSpec *,posRequired ULONG child_index,
        CDrvdPropArray * drgpdpCtxt, ULONG ulOptReq ) const {
    // 在窗口算子之上生成sort算子
    return GPOS_NEW(mp) COrderSpec(mp);
}

	// compute required distribution of the n-th child
	virtual CDistributionSpec *PdsRequired(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CDistributionSpec *pdsRequired,
										   ULONG child_index,
										   CDrvdPropArray *pdrgpdpCtxt,
										   ULONG ulOptReq) const;

	// compute required rewindability of the n-th child
	virtual CRewindabilitySpec *PrsRequired(CMemoryPool *,		   //mp
											CExpressionHandle &,   //exprhdl
											CRewindabilitySpec *,  //prsRequired
											ULONG,			   // child_index
											CDrvdPropArray *,  // pdrgpdpCtxt
											ULONG ulOptReq) const;

	// compute required partition propagation of the n-th child
	virtual CPartitionPropagationSpec *PppsRequired(
		CMemoryPool *,				  //mp,
		CExpressionHandle &,		  //exprhdl,
		CPartitionPropagationSpec *,  //pppsRequired,
		ULONG,						  //child_index,
		CDrvdPropArray *,			  //pdrgpdpCtxt,
		ULONG						  //ulOptReq
	);

// 检查required columns是否都包含output columns
virtual BOOL FProvidesReqdCols(CExpressionHandle &exprhdl,
       CColRefSet *pcrsRequired, ULONG ulOptReq) const {
	ULONG arity = exprhdl.Arity();
	CColRefSet *pcrsChild = exprhdl.DeriveOutputColumns(arity - 1);
	return pcrsChild->ContainsAll(pcrsRequired);
}
 ```



```C++
//----------------------------------------------------------------
// Derived Plan Properties
//----------------------------------------------------------------

// derive sort order from the last child
COrderSpec *PosDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const {
    const ULONG arity = exprhdl.Arity();
	COrderSpec *pos = exprhdl.Pdpplan(arity - 1 /*child_index*/)->Pos();
	pos->AddRef();
	return pos;
}

// derive分布式属性
virtual CDistributionSpec *PdsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const {
    const ULONG arity = exprhdl.Arity();
	CDistributionSpec *pds = exprhdl.Pdpplan(arity - 1 /*child_index*/)->Pds();
	pds->AddRef();
	return pds;
}

// derive rewindability
virtual CRewindabilitySpec *PrsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const;

// derive partition index map
virtual CPartIndexMap * PpimDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
           CDrvdPropCtxt *pdpctxt) const {
    return PpimDeriveCombineRelational(mp, exprhdl);
}

// derive partition filter map
virtual CPartFilterMap * PpfmDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const {
    // combine part filter maps from relational children
    return PpfmDeriveCombineRelational(mp, exprhdl);
}
```

```C++
//-------------------------------------------------------------------------------------
// Enforced Properties
//-------------------------------------------------------------------------------------
// order属性enforce类型(例如Required, Unnecessary等)
virtual CEnfdProp::EPropEnforcingType EpetOrder(
    CExpressionHandle &exprhdl, const CEnfdOrder *peo) const {
	COrderSpec *pos = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Pos();
	if (peo->FCompatible(pos)) {
		// required order will be established by the sequence operator
		return CEnfdProp::EpetUnnecessary;
	}
	return CEnfdProp::EpetRequired;
}

// return rewindability property enforcing type for this operator
virtual CEnfdProp::EPropEnforcingType EpetRewindability(
    CExpressionHandle &exprhdl, const CEnfdRewindability *per) const {
    return CEnfdProp::EpetRequired;
}

virtual BOOL FPassThruStats() const {
    return false;
}
```



## 变换

```C++
// CLogicalSequence逻辑算子转CPhysicalSequence物理算子
CXformImplementSequence::CXformImplementSequence(CMemoryPool *mp)
	: CXformImplementation(// pattern
		  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CLogicalSequence(mp),
			  GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternMultiLeaf(mp)))){}

void CXformImplementSequence::Transform(CXformContext *pxfctxt,
		CXformResult *pxfres, CExpression *pexpr) const {
	CMemoryPool *mp = pxfctxt->Pmp();
	CExpressionArray *pdrgpexpr = pexpr->PdrgPexpr();
	pdrgpexpr->AddRef();

	// create alternative expression
	CExpression *pexprAlt = GPOS_NEW(mp)
		CExpression(mp, GPOS_NEW(mp) CPhysicalSequence(mp), pdrgpexpr);
    //  add alternative to transformation result
    pxfres->Add(pexprAlt);
}

// compute xform promise
EXformPromise CXformImplementSequence::Exfp(CExpressionHandle &) const {
    return CXform::ExfpHigh;
}
```
