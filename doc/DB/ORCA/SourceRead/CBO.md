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

