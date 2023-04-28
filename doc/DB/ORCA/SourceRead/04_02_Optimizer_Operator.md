算子采用`COperator`基类，在其上可以派生出

- `CLogical`：表示逻辑算子。
- `CPhysical`：表示物理算子。
- `CPattern`：
- `CScalar`：

```C++
class COperator : public CRefCount, public DbgPrintMixin<COperator> {
protected:
	// is pattern of xform
	BOOL m_fPattern;
 
    // 对于当前查询, 所有的算子实例中唯一的Operator ID
	ULONG m_ulOpId;
	// 生成唯一的Operator ID, 在每个Operator的构造中加1将其值复制给m_ulOpId
	static ULONG m_aulOpIdCounter;
};
```



```C++
// derive output columns	
virtual CColRefSet *DeriveOutputColumns(CMemoryPool *mp, CExpressionHandle &exprhdl) = 0;
```



```

```

