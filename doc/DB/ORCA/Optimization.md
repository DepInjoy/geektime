```C++
// Container of information passed among expression nodes during property derivation
class CDrvdPropCtxt : public CRefCount
```

```C++
class CExpressionHandle {
private:
    // required properties of attached expr/gexpr; set during required property computation
    CReqdProp *m_prp;
    // array of children's required properties
    CReqdPropArray *m_pdrgprp;

public:
	// recursive property derivation,
	void DeriveProps(CDrvdPropCtxt *pdpctxt);

    // initialize required properties container
    void InitReqdProps(CReqdProp *prpInput);
}
```
## Property
```C++
// Abstract base class for all enforceable properties.
class CEnfdProp {}

// Enforceable order property;
class CEnfdOrder : public CEnfdProp {}

// Enforceable distribution property;
class CEnfdDistribution : public CEnfdProp{}

// Enforceable distribution property
class CEnfdPartitionPropagation : public CEnfdProp {}

// Enforceable rewindability property;
class CEnfdRewindability : public CEnfdProp {}
```

```C++
// Derived logical properties container
class CDrvdPropRelational : public CDrvdProp {}

// Derived scalar properties container.
class CDrvdPropScalar : public CDrvdProp {}

// Derived plan properties container.
class CDrvdPropPlan : public CDrvdProp
```

```C++
// Required plan properties container.
class CReqdPropPlan : public CReqdProp {}

// Required relational properties container.
class CReqdPropRelational : public CReqdProp {}

// Required plan properties container.
class CReqdPropPlan : public CReqdProp {
private:
    CEnfdOrder *m_peo; // required sort order
}
```
EnfdProp负责增加属性算子，DrvdProp用来计算算子所能提供的属性，PropSpec为算子属性描述类，ReqdProp用来计算对孩子的属性请求

Enforceable Property
```plantuml
@startuml
class CEnfdProp {

}
class CEnfdDistribution
class CEnfdOrder
class CEnfdPartitionPropagation
class CEnfdRewindability

CEnfdDistribution <|-- CEnfdProp
CEnfdOrder <|-- CEnfdProp
CEnfdPartitionPropagation <|-- CEnfdProp
CEnfdRewindability <|-- CEnfdProp
note right of CEnfdProp : Abstract base class for all enforceable properties.
@enduml
```

Derived Properties
```plantuml
@startuml
class CDrvdProp {
    + virtual EPropType Ept() = 0
    + virtual void Derive(CMemoryPool *mp, CExpressionHandle &exprhdl, CDrvdPropCtxt *pdppropctxt) = 0
    + virtual BOOL FSatisfies(const CReqdPropPlan *prpp) const = 0
}

class CDrvdPropScalar

note right of CDrvdProp : Abstract base class for all derived properties
note top of CDrvdPropRelational : Derived logical properties container
note top of CDrvdPropPlan : Derived plan properties container

CDrvdPropScalar <|-- CDrvdProp
CDrvdPropRelational <|-- CDrvdProp
CDrvdPropPlan <|-- CDrvdProp

CDrvdPropCtxtRelational <|-- CDrvdPropCtxt
CDrvdPropCtxtPlan <|-- CDrvdPropCtxt
@enduml
```
required properties
```plantuml
class CReqdProp {
    + virtual BOOL FRelational() const
    + virtual BOOL FPlan() const
    + virtual void Compute(CMemoryPool *mp, CExpressionHandle &exprhdl,
            CReqdProp *prpInput, ULONG child_index,
            CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) = 0;
}
class CReqdPropPlan
class CReqdPropRelational

note right of CReqdProp : Abstract base class for all required properties

CReqdProp --|> CReqdPropPlan
CReqdPropPlan *-- CEnfdOrder
CReqdPropPlan *-- CEnfdRewindability
CReqdPropPlan *-- CEnfdDistribution
CReqdPropPlan *-- CEnfdPartitionPropagation
CReqdPropPlan *-- CCTEReq

CEnfdDistribution <|-- CEnfdProp
CEnfdOrder <|-- CEnfdProp
CEnfdPartitionPropagation <|-- CEnfdProp
CEnfdRewindability <|-- CEnfdProp
note right of CEnfdProp : Abstract base class for all enforceable properties.

CReqdProp --|> CReqdPropRelational

```
Property specification
```plantuml
@startuml
class CPropSpec
note right of CPropSpec : Property specification

CRewindabilitySpec <|-- CPropSpec
CDistributionSpec <|-- CPropSpec

CDistributionSpecStrictSingleton <|-- CDistributionSpecSingleton

CDistributionSpecRandom <|-- CDistributionSpec
CDistributionSpecNonSingleton <|-- CDistributionSpec
CDistributionSpecReplicated <|-- CDistributionSpec
CDistributionSpecRouted <|-- CDistributionSpec
CDistributionSpecUniversal  <|-- CDistributionSpec
CDistributionSpecSingleton <|-- CDistributionSpec
CDistributionSpecAny <|-- CDistributionSpec

CDistributionSpecStrictHashed <|-- CDistributionSpecHashed
CDistributionSpecHashed <|-- CDistributionSpecRandom
CDistributionSpecStrictRandom <|-- CDistributionSpecRandom

CDistributionSpecHashedNoOp <|-- CDistributionSpecHashed

COrderSpec <|-- CPropSpec
CPartitionPropagationSpec <|-- CPropSpec
@enduml
```