先看一个窗口例子了解窗口在ORCA中的代数表表达式，例如

```sql
EXPLAIN
select sum(a) over(
    	partition by a 							-- 分区条件,对数据分组
    	order by b								-- 分区内数据排序列
    	ROWS between 1 preceding and 1 following   -- Frame
	)
	from test_a
```

```
Algebrized query: 
+--CLogicalSequenceProject (Partition By Keys:HASHED: [ CScalarIdent "a" (0), nulls colocated ], opfamilies: (1977,1.0),, Order Spec:[( (97,1.0), "b" (1), NULLsLast ) ], WindowFrame Spec:[[Rows, Trail: Bounded Preceding +--CScalarConst (1)
, Lead: Bounded Following +--CScalarConst (1)
, Nulls]])
   |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}
   +--CScalarProjectList
      +--CScalarProjectElement "sum" (11)
         +--CScalarWindowFunc (sum , Agg: true , Distinct: false , StarArgument: false , SimpleAgg: true)
            +--CScalarIdent "a" (0)
```



如果有多个窗口

```sql 
EXPLAIN
select sum(a) over(partition by a order by b ROWS between 1 preceding and 1 following),
	count() over (partition by a)
from test_a
```

```
Algebrized query: 
+--CLogicalSequenceProject (Partition By Keys:HASHED: [ CScalarIdent "a" (0), nulls colocated ], opfamilies: (1977,1.0),, Order Spec:[<empty>], WindowFrame Spec:[[Range, Trail: Unbounded Preceding, Lead: Current, Nulls]])
   |--CLogicalSequenceProject (Partition By Keys:HASHED: [ CScalarIdent "a" (0), nulls colocated ], opfamilies: (1977,1.0),, Order Spec:[( (97,1.0), "b" (1), NULLsLast ) ], WindowFrame Spec:[[Rows, Trail: Bounded Preceding +--CScalarConst (1)
, Lead: Bounded Following +--CScalarConst (1)
, Nulls]])
   |  |--CLogicalGet "test_a" ("test_a"), Columns: ["a" (0), "b" (1), "c" (2), "d" (3), "ctid" (4), "xmin" (5), "cmin" (6), "xmax" (7), "cmax" (8), "tableoid" (9), "gp_segment_id" (10)] Key sets: {[4,10]}
   |  +--CScalarProjectList
   |     +--CScalarProjectElement "sum" (11)
   |        +--CScalarWindowFunc (sum , Agg: true , Distinct: false , StarArgument: false , SimpleAgg: true)
   |           +--CScalarIdent "a" (0)
   +--CScalarProjectList
      +--CScalarProjectElement "count" (12)
         +--CScalarWindowFunc (count , Agg: true , Distinct: false , StarArgument: false , SimpleAgg: true)
```



# DXL转化Expr

窗口在DXL中的表示

```C++
class CDXLLogicalWindow : public CDXLLogical;
class CDXLScalarProjList : public CDXLScalar;
// scalar project列表元素，存储表达式和别名(alias)
class CDXLScalarProjElem : public CDXLScalar;
// 窗口函数在DXL中的表示
class CDXLScalarWindowRef : public CDXLScalar;
class CDXLScalarIdent : public CDXLScalar;
```

DXL结构会转化成逻辑算子构成的代数表达式，窗口的逻辑算子表示

```C++
class CLogicalSequenceProject : public CLogicalUnary {
private:
	CDistributionSpec *m_pds; // partition by keys
	COrderSpecArray *m_pdrgpos; // order specs
	CWindowFrameArray *m_pdrgpwf; // frames
	BOOL m_fHasOrderSpecs; // order specs是否为空
	BOOL m_fHasFrameSpecs; // frame specs是否为空
};

class CScalarProjectList : public CScalar;
// 用于定义等价于scalar表达式的列引用
class CScalarProjectElement : public CScalar {
private:
    // defined column reference
    CColRef *m_pcr;
};

// scalar window function
class CScalarWindowFunc : public CScalarFunc;
// scalar identifier operator
class CScalarIdent : public CScalar {
private:
    const CColRef *m_pcr;  // column
};

// Frame表示结构
class CWindowFrame : public CRefCount;
// partition by分区条件
class CDistributionSpec : public CPropSpec;
```



```C++
/**
	根据DXL logical window创建CLogicalSequenceProject逻辑算子
    CDXLLogicalWindow								CLogicalSequenceProject
        |-- CDXLScalarProjList			   			  |-- Relation(eg,CLogicalGet)
        	+-- CDXLScalarProjElem	        ->			   	+-- CScalarProjectList
            	+-- CDXLScalarWindowRef							+-- CScalarProjectElement
                	+-- CDXLScalarIdent								+-- CScalarWindowFunc
         +-- Relation(eg,CDXLLogicalGet)								+-- CScalarIdent
*/
CExpression * CTranslatorDXLToExpr::PexprLogicalSeqPr(const CDXLNode *dxlnode) {
	CDXLLogicalWindow *pdxlopWindow = CDXLLogicalWindow::Cast(dxlnode->GetOperator());
    // 1. 第1个孩子窗口的输入关系，调用PexprLogical将DXL输入转换成Logical算子
	CDXLNode *pdxlnWindowChild = (*dxlnode)[1];
	CExpression *pexprWindowChild = PexprLogical(pdxlnWindowChild);

    // 用于生成级联窗口, 维护window spec position和project element间的映射
   	// CDXLScalarWindowRef存有m_win_spec_pos(表示在父窗口算子中的window spec position)
    // <window spec position, project element expr>
	UlongToExprArrayMap *phmulpdrgpexpr = GPOS_NEW(m_mp) UlongToExprArrayMap(m_mp);

    // 2. 输入的DXL Scalar转换成scalar算子
    //    (这里生成CScalarProjectElement, CScalarWindowFunc)
    //    2.1 应是CDXLScalarProjList,其有1个CDXLScalarProjElem表示的孩子
	CDXLNode *pdxlnPrL = (*dxlnode)[0];
	const ULONG arity = pdxlnPrL->Arity();
	for (ULONG ul = 0; ul < arity; ul++) {
         // 2.2 获取到CDXLScalarProjElem
		CDXLNode *pdxlnProjElem = (*pdxlnPrL)[ul];
         // 获取CDXLScalarProjElem的孩子,project list应该normalize
         // 应是CDXLScalarWindowRef或CDXLScalarIdent(?)
		CDXLNode *pdxlnPrElChild = (*pdxlnProjElem)[0];
		CDXLScalarProjElem *pdxlopPrEl = CDXLScalarProjElem::Cast(
            	pdxlnProjElem->GetOperator());
		
         // 2.3 将dxl的窗口函数转换成CScalarWindowFunc
		if (EdxlopScalarWindowRef == pdxlnPrElChild->GetOperator()->GetDXLOperator()) {
			// 调用Pexpr-> PexprScalar -> PexprWindowFunc
             // 由CTranslatorDXLToExpr::PexprWindowFunc实现具体转换动作
             // 将CDXLScalarWindowRef输入, 生成CScalarWindowFunc
			CDXLScalarWindowRef *pdxlopWindowRef =
                	CDXLScalarWindowRef::Cast(pdxlnPrElChild->GetOperator());
			CExpression *pexprScWindowFunc = Pexpr(pdxlnPrElChild);

             // 从CDXLScalarWindowRef中获取元数据object id(MDId)
             // 生成column reference(CColRef),并生成CScalarProjectElement算子
			CScalar *popScalar = CScalar::PopConvert(pexprScWindowFunc->Pop());
			IMDId *mdid = popScalar->MdidType();
			const IMDType *pmdtype = m_pmda->RetrieveType(mdid);
			CName name(pdxlopPrEl->GetMdNameAlias()->GetMDName());
			CColRef *colref = m_pcf->PcrCreate(pmdtype, popScalar->TypeModifier(), name);
			CScalarProjectElement *popScPrEl = GPOS_NEW(m_mp)CScalarProjectElement(m_mp, colref);

			// 存储<dxl colid -> colref>映射
			BOOL fInserted GPOS_ASSERTS_ONLY = m_phmulcr->Insert(
				GPOS_NEW(m_mp) ULONG(pdxlopPrEl->Id()), colref);

			// 生成project element表达式
             // 	popScPrEl(CScalarProjectElement)
             //			|-- pexprScWindowFunc(CScalarWindowFunc)
			CExpression *pexprProjElem =
             	 GPOS_NEW(m_mp) CExpression(m_mp, popScPrEl, pexprScWindowFunc);

			// add the created project element to the project list of the window node
			ULONG ulSpecPos = pdxlopWindowRef->GetWindSpecPos();
			const CExpressionArray *pdrgpexpr = phmulpdrgpexpr->Find(&ulSpecPos);
			if (nullptr == pdrgpexpr) {
				CExpressionArray *pdrgpexprNew = GPOS_NEW(m_mp) CExpressionArray(m_mp);
				pdrgpexprNew->Append(pexprProjElem);
				BOOL fInsert GPOS_ASSERTS_ONLY = phmulpdrgpexpr->Insert(
					GPOS_NEW(m_mp) ULONG(ulSpecPos), pdrgpexprNew);
			} else {
				const_cast<CExpressionArray *>(pdrgpexpr)->Append(pexprProjElem);
			}
		}
	}

	// 3. 创建窗口算子(CLogicalSequenceProject)
	CExpression *pexprLgSequence = nullptr;
	UlongToExprArrayMapIter hmiterulpdrgexpr(phmulpdrgpexpr);
    // 对上面创建的<window spec position, project element expr>map遍历
	while (hmiterulpdrgexpr.Advance()) {
         // 获取window spec position和对应的project element
		ULONG ulPos = *(hmiterulpdrgexpr.Key());
         const CExpressionArray *pdrgpexpr = hmiterulpdrgexpr.Value();
		CDXLWindowSpec *pdxlws = pdxlopWindow->GetWindowKeyAt(ulPos);

         // 3.1 生成project list
         //     CScalarProjectList
         //			|-- CScalarProjectElement(即hmiterulpdrgexpr.Value)
		CScalarProjectList *popPrL = GPOS_NEW(m_mp) CScalarProjectList(m_mp);
		CExpression *pexprProjList = GPOS_NEW(m_mp) CExpression(
			m_mp, popPrL, const_cast<CExpressionArray *>(pdrgpexpr));

        // 3.2 创建partition by分区条件
		CColRefArray *colref_array = PdrgpcrPartitionByCol(pdxlws->GetPartitionByColIdArray());
		CDistributionSpec *pds = nullptr;
		if (0 < colref_array->Size()) {
			CExpressionArray *pdrgpexprScalarIdents =
				CUtils::PdrgpexprScalarIdents(m_mp, colref_array);
			pds = CDistributionSpecHashed::MakeHashedDistrSpec(
				m_mp, pdrgpexprScalarIdents, true /* fNullsCollocated */,
				nullptr /* pdshashedEquiv */, nullptr /* opfamilies */);
			if (nullptr == pds) {
				GPOS_RAISE(gpdxl::ExmaMD, gpdxl::ExmiMDObjUnsupported,
					GPOS_WSZ_LIT("no default hash opclasses found in window function"));
			}
		} else { // 无partition-by分区列,生成gather node
			pds = GPOS_NEW(m_mp) CDistributionSpecSingleton(
				CDistributionSpecSingleton::EstMaster);
		}
		colref_array->Release();

        // 3.3 创建Frame,若dxl中Frame不为空,创建CWindowFrame，否则创建空的CWindowFrame
		CWindowFrameArray *pdrgpwf = GPOS_NEW(m_mp) CWindowFrameArray(m_mp);
		CWindowFrame *pwf = nullptr;
		if (nullptr != pdxlws->GetWindowFrame()) {
			pwf = Pwf(pdxlws->GetWindowFrame());
		} else {
			pwf = const_cast<CWindowFrame *>(CWindowFrame::PwfEmpty());
			pwf->AddRef();
		}
		pdrgpwf->Append(pwf);

        // 3.4 创建order by spec
		COrderSpecArray *pdrgpos = GPOS_NEW(m_mp) COrderSpecArray(m_mp);
		if (nullptr != pdxlws->GetSortColListDXL()) {
			COrderSpec *pos = Pos(pdxlws->GetSortColListDXL());
			pdrgpos->Append(pos);
		} else {
			pdrgpos->Append(GPOS_NEW(m_mp) COrderSpec(m_mp));
		}

        // 3.4 生成最终的CLogicalSequenceProject算子和表达式
		CLogicalSequenceProject *popLgSequence =
			GPOS_NEW(m_mp) CLogicalSequenceProject(m_mp, pds, pdrgpos, pdrgpwf);
		pexprLgSequence = GPOS_NEW(m_mp)
			CExpression(m_mp, popLgSequence, pexprWindowChild, pexprProjList);
		pexprWindowChild = pexprLgSequence;
	}
	phmulpdrgpexpr->Release();

	return pexprLgSequence;
}

// CTranslatorDXLToExpr::PexprWindowFunc将CDXLScalarWindowRef输入, 生成CScalarWindowFunc
CExpression * CTranslatorDXLToExpr::PexprWindowFunc(const CDXLNode *pdxlnWindowRef) {
	CDXLScalarWindowRef *pdxlopWinref = CDXLScalarWindowRef::Cast(pdxlnWindowRef->GetOperator());

	IMDId *mdid_func = pdxlopWinref->FuncMdId();
	mdid_func->AddRef();
	CWStringConst *str_name = GPOS_NEW(m_mp) CWStringConst(m_mp,
		CMDAccessorUtils::PstrWindowFuncName(m_pmda, mdid_func)->GetBuffer());
    // 获取window stage
	CScalarWindowFunc::EWinStage ews = Ews(pdxlopWinref->GetDxlWinStage());
	IMDId *mdid_return_type = pdxlopWinref->ReturnTypeMdId();
	mdid_return_type->AddRef();
	
    // 生成CScalarWindowFunc
	CScalarWindowFunc *popWindowFunc = GPOS_NEW(m_mp)
		CScalarWindowFunc(m_mp, mdid_func, mdid_return_type, str_name, ews,
						  pdxlopWinref->IsDistinct(), pdxlopWinref->IsStarArg(),
						  pdxlopWinref->IsSimpleAgg());

    // translate窗口函数入参信息
	CExpression *pexprWindowFunc = nullptr;
	if (0 < pdxlnWindowRef->Arity()) {
		CExpressionArray *pdrgpexprArgs = PdrgpexprChildren(pdxlnWindowRef);
		pexprWindowFunc = GPOS_NEW(m_mp) CExpression(m_mp, popWindowFunc, pdrgpexprArgs);
	} else { // 无参数
		pexprWindowFunc = GPOS_NEW(m_mp) CExpression(m_mp, popWindowFunc);
	}
	return pexprWindowFunc; 
}
```

# 预处理

```
CExpressionPreprocessor::PexprPreprocess
	-> CWindowPreprocessor::PexprPreprocess
```

```C++
// 没有写出满足的实际示例,之后再完善
class CWindowPreprocessor
```

# 算子

## 逻辑算子

```C++
class CLogicalSequenceProject : public CLogicalUnary {
private:
	CDistributionSpec *m_pds; // 分区条件(partition by)
	COrderSpecArray *m_pdrgpos; // order by specs
	CWindowFrameArray *m_pdrgpwf; // frames

    // 标识是否有非空order specs,如果没有order spec, orca会插入一个空order spec
    // 参见CLogicalSequenceProject::SetHasOrderSpecs
	BOOL m_fHasOrderSpecs;
    // 标识是否有非空的frame, 如果没有frame, orca会插入一个空frame
    // 参见CLogicalSequenceProject::SetHasFrameSpecs
	BOOL m_fHasFrameSpecs;
}

class CDistributionSpec : public CPropSpec;
class COrderSpec : public CPropSpec;
/**
	ROWS between 1 preceding and 1 following

	打印出结果和数据结构的映射
	Rows,						  -- m_efs(FrameSpe)
	
	Trail: Bounded Preceding		-- m_efbTrailing(type of trailing edge)
		+--CScalarConst (1), 		-- m_pexprTrailing(scalar value of trailing edge)
	
	Lead: Bounded Following			-- m_efbLeading(type of leading edge)
		+--CScalarConst (1)		    -- m_pexprLeading(scalar value of leading edge)
	
	, Nulls						   -- m_efes(exclusion strategy)
*/
class CWindowFrame : public CRefCount{
private:
	const EFrameSpec m_efs; // 标识是Rows还是Rangs

	// type of leading edge
	const EFrameBoundary m_efbLeading;
    // scalar value of leading edge
	CExpression *m_pexprLeading;
 
	// type of trailing edge
	const EFrameBoundary m_efbTrailing;
	// scalar value of trailing edge
	CExpression *m_pexprTrailing;

	// exclusion strategy
	const EFrameExclusionStrategy m_efes;

	// columns used by frame edges
	CColRefSet *m_pcrsUsed;
	static const CWindowFrame m_wfEmpty; // empty frame单例
}
```

```C++
// Return a copy of the operator with remapped columns
virtual COperator * CLogicalSequenceProject::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist) {
    // 对DistributionSpec, OrderSpec, Frames分别Copy Remapped Columns
		......
	return GPOS_NEW(mp) CLogicalSequenceProject(mp, pds, pdrgpos, pdrgpwf);
}

// 如果可以将它的孩子上拉至该算子之上,返回true
virtual BOOL FCanPullProjectionsUp(ULONG) const {
    return false;
}
```

derives关系属性

```C++
// derive output columns
virtual CColRefSet *DeriveOutputColumns(CMemoryPool *mp,
	CExpressionHandle &exprhdl) {
    CColRefSet *pcrs = GPOS_NEW(mp) CColRefSet(mp);
    
    // 下层关系+scalar孩子的output columns
	pcrs->Union(exprhdl.DeriveOutputColumns(0));
	pcrs->Union(exprhdl.DeriveDefinedColumns(1));
    
    return pcrs;
}

// derive outer references
// outer references是scalar孩子使用的列，但不含在relation孩子的output columns
virtual CColRefSet *DeriveOuterReferences(CMemoryPool *mp,
		CExpressionHandle &exprhdl) {
    CColRefSet *outer_refs = CLogical::DeriveOuterReferences(mp,
			exprhdl, m_pcrsLocalUsed);
	return outer_refs;
}

// dervive keys
virtual CKeyCollection *DeriveKeyCollection(
    CMemoryPool *mp, CExpressionHandle &exprhdl) const {
    return PkcDeriveKeysPassThru(exprhdl, 0 /* ulChild */);
}

// derive max card
virtual CMaxCard DeriveMaxCard(CMemoryPool *mp,
	CExpressionHandle &exprhdl) const {
    return exprhdl.DeriveMaxCard(0);
}

// derive constraint property
virtual CPropConstraint *
DerivePropertyConstraint(CMemoryPool *,	CExpressionHandle &exprhdl) const {
    return PpcDeriveConstraintPassThru(exprhdl, 0 /*ulChild*/);
}
```

transform变换

```C++
// 可选的xform变换
CXformSet * CLogicalSequenceProject::PxfsCandidates(CMemoryPool *mp) const {
	CXformSet *xform_set = GPOS_NEW(mp) CXformSet(mp);
    // 去关联
	(void) xform_set->ExchangeSet(CXform::ExfSequenceProject2Apply);
    // 逻辑算子转物理算子
	(void) xform_set->ExchangeSet(CXform::ExfImplementSequenceProject);
	return xform_set;
}
```

derive statistics

```C++
IStatistics * CLogicalSequenceProject::PstatsDerive(CMemoryPool *mp,
	CExpressionHandle &exprhdl, IStatisticsArray *stats_ctxt) const {
	return PstatsDeriveProject(mp, exprhdl);
}

IStatistics * CLogicalUnary::PstatsDeriveProject(CMemoryPool *mp,
		CExpressionHandle &exprhdl,UlongToIDatumMap *phmuldatum) const {
    // 获取第0个孩子的统计信息
	IStatistics *child_stats = exprhdl.Pstats(0);
	CReqdPropRelational *prprel = 
        	CReqdPropRelational::GetReqdRelationalProps(exprhdl.Prp());
    // stat columns
	CColRefSet *pcrs = prprel->PcrsStat();
	ULongPtrArray *colids = GPOS_NEW(mp) ULongPtrArray(mp);
    // 从ColRefSet中提取真实的column id
	pcrs->ExtractColIds(mp, colids);

    // 计算统计信息
	IStatistics *stats = CProjectStatsProcessor::CalcProjStats(
		mp, dynamic_cast<CStatistics *>(child_stats), colids, phmuldatum);

	colids->Release();
	return stats;
}
```

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
// 窗口函数含子查询时, 进行子查询扁平化, 去关联
// Transform的具体实现由子类CXformSubqueryUnnest::Transform完成
CXformSequenceProject2Apply::CXformSequenceProject2Apply(CMemoryPool *mp):// pattern
  CXformSubqueryUnnest(GPOS_NEW(mp) CExpression(
      mp, GPOS_NEW(mp) CLogicalSequenceProject(mp),
      GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
      GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // project list
      )){}
```



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



