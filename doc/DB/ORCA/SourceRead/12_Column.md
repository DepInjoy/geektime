```C++
// 由CColumnFactory创建,参见CColumnFactory::PcrCreate
class CColRef : public gpos::DbgPrintMixin<CColRef> {
private:
	const IMDType *m_pmdtype; // type information
	const INT m_type_modifier; // type modifier
	
    const CName *m_pname; // SQL alias or artificial name
	// track the usage of colref (used/unused/unknown)
	EUsedStatus m_used;
	IMDId *m_mdid_table; // table info
public:
    // id用于HashValue计算，在CColumnFactory::PcrCreate创建时赋值
    // 在CColumnFactory有m_aul变量，初始化为0,每新建一个ColRef自增一
    const ULONG m_id;
	enum Ecolreftype {
		EcrtTable,
		EcrtComputed,
		EcrtSentinel
	};
};

class CColRefComputed : public CColRef {
public:
    	....
	CColRef::Ecolreftype Ecrt() const override {
		return CColRef::EcrtComputed;
	}
}

// Column reference for base table columns
class CColRefTable : public CColRef {
public:
	CColRef::Ecolreftype Ecrt() const override {
		return CColRef::EcrtTable;
	}
}
```



```C++
class CColumnFactory {
private:
    // scalar的project element是对应于一个Colref
    // 这里存储Computed Column和expression used column reference的映射
	ColRefToColRefSetMap *m_phmcrcrs{nullptr};

	// 每新创建一个ColRef自增1,将其作为ColRef的id用于HashValue计算
	ULONG m_aul{0};
	CSyncHashtable<CColRef, ULONG> m_sht;
}
```

每个`CColRef`都有一个`id`用于计算Hash Key。

```C++
ULONG CColRef::HashValue(const CColRef *colref) {
	ULONG id = colref->Id();
	return gpos::HashValue<ULONG>(&id);
}
```



```C++
// Add the map between computed column and its used columns
void CColumnFactory::AddComputedToUsedColsMap(CExpression *pexpr) {
	const CScalarProjectElement *popScPrEl = CScalarProjectElement::PopConvert(pexpr->Pop());
    // CScalarProjectElement的colRef
	CColRef *pcrComputedCol = popScPrEl->Pcr();
	CColRefSet *pcrsUsed = pexpr->DeriveUsedColumns();
	if (nullptr != pcrsUsed && 0 < pcrsUsed->Size()) {
		BOOL fres GPOS_ASSERTS_ONLY = m_phmcrcrs->Insert(
			pcrComputedCol, GPOS_NEW(m_mp) CColRefSet(m_mp, *pcrsUsed));
	}
}

// 根据computed column的id查找used column references的ColRefSet
const CColRefSet * CColumnFactory::PcrsUsedInComputedCol(const CColRef *colref) {
	const CColRefSet *pcrs = m_phmcrcrs->Find(colref);
	return pcrs;
}
```



```C++
// 将ColRef按位压缩成BitSet,其中BitVector的size为GPOPT_COLREFSET_SIZE(1024)
// 对外提供了一系列的集合操作,可以实现两个CColRefSet的交,并差等一些列运算
class CColRefSet : public CBitSet, public DbgPrintMixin<CColRefSet> {
	friend class CColRefSetIter;
public:
	explicit CColRefSet(CMemoryPool *mp,
						ULONG ulSizeBits = GPOPT_COLREFSET_SIZE);
	CColRefSet(CMemoryPool *mp, const CColRefArray *colref_array,
			   ULONG ulSizeBits = GPOPT_COLREFSET_SIZE);
}
```

