
```C++
// Create a logical sequence expr from a DXL logical window
CExpression *
CTranslatorDXLToExpr::PexprLogicalSeqPr(const CDXLNode *dxlnode)
```
```C++
CExpression *
CWindowPreprocessor::PexprPreprocess(CMemoryPool *mp, CExpression *pexpr)
```
```plantuml
@startuml
class CDXLLogicalWindow {
    - CDXLWindowSpecArray *m_window_spec_array
}

class CDXLWindowSpec {
    - ULongPtrArray *m_partition_by_colid_array
    - CMDName *m_mdname
    - CDXLNode *m_sort_col_list_dxlnode
    - CDXLWindowFrame *m_window_frame
}

class CDXLWindowFrame {
    - EdxlFrameSpec m_dxl_win_frame_spec;
    - EdxlFrameExclusionStrategy m_dxl_frame_exclusion_strategy;
    - CDXLNode *m_dxlnode_leading;
    - CDXLNode *m_dxlnode_trailing;
}

class CDXLLogical
class CDXLOperator
CDXLLogicalWindow <|-- CDXLLogical
CDXLLogicalWindow *- CDXLWindowSpec
CDXLWindowSpec *- CDXLWindowFrame
CDXLLogical <|-- CDXLOperator
@enduml
```

```plantuml
@startuml
class CLogicalSequenceProject {
    - CDistributionSpec *m_pds;
    - COrderSpecArray *m_pdrgpos;
    - CWindowFrameArray *m_pdrgpwf;
    - BOOL m_fHasOrderSpecs;
    - BOOL m_fHasFrameSpecs;
}

class CDistributionSpec {

}

class CWindowFrame {

}

class COrderSpec {

}

CLogicalSequenceProject <|- CLogicalUnary
CLogicalSequenceProject *- CDistributionSpec
CWindowFrame -* CLogicalSequenceProject

CLogicalSequenceProject *-- COrderSpec
COrderSpec <|-- CPropSpec

CDistributionSpec <|-- CPropSpec
CLogicalUnary <|- CLogical
CLogical <|- COperator
@enduml
```

```C++
// Create a scalar window function expression from a DXL window ref
//  CDXLScalarWindowRef -> CScalarWindowFunc Operator
CExpression *
CTranslatorDXLToExpr::PexprWindowFunc(const CDXLNode *pdxlnWindowRef)
```
```plantuml
@startuml
class CDXLScalarWindowRef {
    - IMDId *m_func_mdid
	- IMDId *m_return_type_mdid;
	- BOOL m_is_distinct;
	- BOOL m_is_star_arg;
	- BOOL m_is_simple_agg;
	- EdxlWinStage m_dxl_win_stage;
	- ULONG m_win_spec_pos;
}

enum EdxlWinStage {
	+ EdxlwinstageImmediate = 0,
	+ EdxlwinstagePreliminary,
	+ EdxlwinstageRowKey,
	+ EdxlwinstageSentinel
}

CDXLScalarWindowRef <|-- CDXLScalar
CDXLScalarWindowRef *- EdxlWinStage
@enduml
```

```plantuml
@startuml
class CScalarWindowFunc {
    - EWinStage m_ewinstage;
	- BOOL m_is_distinct;
	- BOOL m_is_star_arg;
	- BOOL m_is_simple_agg;
	- BOOL m_fAgg;
}

enum EWinStage {
    + EwsImmediate,
    + EwsPreliminary,
    + EwsRowKey,
    + EwsSentinel
}

CScalarWindowFunc *- EWinStage
CScalarWindowFunc <|-- CScalarFunc
CScalarFunc <|-- CScalar
CScalar <|-- COperator
@enduml
```

```C++
class CXformSequenceProject2Apply : public CXformSubqueryUnnest

CXformImplementSequenceProject
```