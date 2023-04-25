# Required Property

算子的`Required`属性采用`CReqdProp`为基类，在其上派生出

- `CReqdPropRelational`：用于逻辑算子output列计算(`CReqdPropRelational::Compute`)。

- `CReqdPropPlan`：用于物理算子属性计算(`CReqdPropPlan::Compute`)，物理算子属性包含：
    - `CEnfdOrder`：排序(`Order`)，
    - `CEnfdDistribution`：数据分布(`Distribute`)。
    - `CEnfdRewindability`
    - `CEnfdPartitionPropagation`

```C++
class CReqdPropRelational : public CReqdProp {
private:
	// required stat columns
	CColRefSet *m_pcrsStat{nullptr};

	// predicate on partition key
	CExpression *m_pexprPartPred{nullptr};
};

class CReqdPropPlan : public CReqdProp {
private:
	// required columns
	CColRefSet *m_pcrs{nullptr};

	// required sort order
	CEnfdOrder *m_peo{nullptr}; 
	// required distribution
	CEnfdDistribution *m_ped{nullptr};
	// required rewindability
	CEnfdRewindability *m_per{nullptr};
	// required partition propagation
	CEnfdPartitionPropagation *m_pepp{nullptr};

	// required ctes
	CCTEReq *m_pcter{nullptr};
};
```

`Required`属性通过算子基类`Operator`定义纯虚的`PrpCreate`，在逻辑算子`CLogical`和物理算子`CPhysical`自定义`PrpCreate()`接口创建属性，其中`CPattern`和`CScalar`无`Required`属性。相关接口定义如下：

```C++
virtual CReqdProp *PrpCreate(CMemoryPool *mp) const = 0;
// 物理算子对应CReqdPropPlan
CReqdProp * CPhysical::PrpCreate(CMemoryPool *mp) const {
	return GPOS_NEW(mp) CReqdPropPlan();
}

// 逻辑算子对应CReqdPropRelational
CReqdProp * CLogical::PrpCreate(CMemoryPool *mp) const {
	return GPOS_NEW(mp) CReqdPropRelational();
}

// Pattern和Scalar无Required属性
CDrvdProp * CPattern::PdpCreate(CMemoryPool *) const {
	GPOS_ASSERT(!"Cannot derive properties on pattern");
	return nullptr;
}
CReqdProp * CScalar::PrpCreate(CMemoryPool *) const {
	GPOS_ASSERT(!"Cannot compute required properties on scalar");
	return nullptr;
}
```



## Property Enforcement

上面可以看到物理算子的`Required`属性由一系列的`Enforce Property`组成。

分布式属性采用`CEnfdDistribution`结构表示，其中`CDistributionSpec`表示`required`分发方式，支持

```C++
// Enforceable distribution property
class CEnfdDistribution : public CEnfdProp {
private:
	// required distribution
	CDistributionSpec *m_pds;

	// distribution matching type
	EDistributionMatching m_edm;
};

// 
class CDistributionSpec : public CPropSpec;
class CDistributionSpecAny : public CDistributionSpec;
class CDistributionSpecNonSingleton : public CDistributionSpec;
class CDistributionSpecRandom : public CDistributionSpec;
class CDistributionSpecReplicated : public CDistributionSpec;
class CDistributionSpecRouted : public CDistributionSpec;
class CDistributionSpecSingleton : public CDistributionSpec;
class CDistributionSpecUniversal : public CDistributionSpec;
class CDistributionSpecHashed : public CDistributionSpecRandom;
```



```C++
class CEnfdOrder : public CEnfdProp
```



```C++
class CEnfdPartitionPropagation : public CEnfdProp
```



```C++
class CEnfdRewindability : public CEnfdProp
```

# Derived property

属性`Derive`自底向上进行，该属性表达采用`CDrvdProp`为基类，在其上派生出`Scalar`、`Relation`和`Plan`三种属性

1. `CDrvdPropScalar`
2. `CDrvdPropRelational`
3. `CDrvdPropPlan`

```C++
class CDrvdProp {
public:
    // 基类中定义纯虚函数
    // 其上派生的父类负责填充相应的属性
	virtual void Derive(CMemoryPool *mp, CExpressionHandle &exprhdl,
                        CDrvdPropCtxt *pdppropctxt) = 0;

	// check for satisfying required plan properties
	virtual BOOL FSatisfies(const CReqdPropPlan *prpp) const = 0;
};

class CDrvdPropScalar : public CDrvdProp;
class CDrvdPropRelational : public CDrvdProp;
class CDrvdPropPlan : public CDrvdProp;
```

`Derived`属性通过算子基类`Operator`定义纯虚的`PdpCreate`，在Scalar算子，逻辑算子`CLogical`和物理算子`CPhysical`自定义`PdpCreate()`接口创建属性。相关接口定义如下：

```C++
CDrvdProp *CScalar::PdpCreate(CMemoryPool *mp) const {
	return GPOS_NEW(mp) CDrvdPropScalar(mp);
}

CDrvdProp *CLogical::PdpCreate(CMemoryPool *mp) const {
	return GPOS_NEW(mp) CDrvdPropRelational(mp);
}

CDrvdProp *CPhysical::PdpCreate(CMemoryPool *mp) const {
	return GPOS_NEW(mp) CDrvdPropPlan();
}
```



```C++
class CDrvdProp {
public:
	virtual void Derive(CMemoryPool *mp, CExpressionHandle &exprhdl,
                        CDrvdPropCtxt *pdppropctxt) = 0;

	// check for satisfying required plan properties
	virtual BOOL FSatisfies(const CReqdPropPlan *prpp) const = 0;
};

class CDrvdPropScalar : public CDrvdProp
class CDrvdPropRelational : public CDrvdProp {
private:
	// bitset representing whether property has been derived
	CBitSet *m_is_prop_derived;

	// output columns
	CColRefSet *m_pcrsOutput;

	// columns not defined in the underlying operator tree
	CColRefSet *m_pcrsOuter;

	// output columns that do not allow null values
	CColRefSet *m_pcrsNotNull;

	// columns from the inner child of a correlated-apply expression that can be used above the apply expression
	CColRefSet *m_pcrsCorrelatedApply;

	// key collection
	CKeyCollection *m_pkc;

	// functional dependencies
	CFunctionalDependencyArray *m_pdrgpfd;

	// max card
	CMaxCard m_maxcard;

	// join depth (number of relations in underlying tree)
	ULONG m_ulJoinDepth;

	// partition table consumers
	CPartInfo *m_ppartinfo;

	// constraint property
	CPropConstraint *m_ppc;

	// function properties
	CFunctionProp *m_pfp;

	CTableDescriptor *m_table_descriptor;
};

class CDrvdPropPlan : public CDrvdProp {
private:
	// derived sort order
	COrderSpec *m_pos{nullptr};
	// derived distribution
	CDistributionSpec *m_pds{nullptr};
	// derived rewindability
	CRewindabilitySpec *m_prs{nullptr};
	// derived partition propagation spec
	CPartitionPropagationSpec *m_ppps{nullptr};

	// derived cte map
	CCTEMap *m_pcm{nullptr};
};
```



```C++
//		Set required properties of attached expr/gexpr, and compute required
//		properties of all children
void CExpressionHandle::ComputeReqdProps(CReqdProp *prpInput, ULONG ulOptReq) {
	InitReqdProps(prpInput);
	const ULONG arity = Arity();
	for (ULONG ul = 0; ul < arity; ul++) {
		ComputeChildReqdProps(ul, nullptr /*pdrgpdpCtxt*/, ulOptReq);
	}
}

// Compute required properties of the n-th child
void CExpressionHandle::ComputeChildReqdProps(ULONG child_index,
			CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) {
	CReqdProp *prp = m_prp;
	if (FScalarChild(child_index)) {
		// use local reqd properties to fill scalar child entry in children array
		prp->AddRef();
	} else {
		// compute required properties based on child type
		prp = Pop()->PrpCreate(m_mp);
		prp->Compute(m_mp, *this, m_prp, child_index, pdrgpdpCtxt, ulOptReq);
	}
	m_pdrgprp->Replace(child_index, prp);
}
```





```C++
// CEnfdProp	
// append enforcers to dynamic array for the given plan properties
void AppendEnforcers(CMemoryPool *mp, CReqdPropPlan *prpp,
    CExpressionArray *pdrgpexpr,  // array of enforcer expressions
    CExpression *pexprChild,	  // leaf in the target group where
                                  // enforcers will be added
    CEnfdProp::EPropEnforcingType epet, CExpressionHandle &exprhdl) const {
    if (FEnforce(epet)) {
        Pps()->AppendEnforcers(mp, exprhdl, prpp, pdrgpexpr, pexprChild);
    }
}
```



