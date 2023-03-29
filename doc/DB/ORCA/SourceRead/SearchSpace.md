ORCA采用Memo数据结构来存储执行计划的搜索空间，它由一系列的Group组成。Memo存有一个HashTable存储，其Key是GroupExpression的HashValue，Value是GroupExpression，当向memo中插入GroupExpression时，会先在HashTable中进行查找如果存在，就不需要新建Group来节省内存空间。

# Memo

## 数据结构

Memo数据结构的表示

```C++
class CMemo {
private:
    // 根Group
    CGroup *m_pgroupRoot;

    // 所有GroupExpression的HashTable，
    // 插入GroupExpression时先检查是否已经存在，如果存在不需要新建Group可以节省内存空间
    // 参见PgroupInsert
	CSyncHashtable<CGroupExpression, CGroupExpression> m_sht;
}

```

## 数据操作

Memo结构对外提供的数据操作API:

```C++
设置跟Group
void SetRoot(CGroup *pgroup);

将GroupExpression插入Hash Table,如果HashTable中已有该GroupExpression则直接将查找到的GroupExpression中的Group返回，值得注意的是，如果新建了Group，会调用Expression的PdpDerive接口来derive属性
CGroup *PgroupInsert(CGroup *pgroupTarget, CExpression *pexprOrigin,
                     CGroupExpression *pgexpr);

从当前的搜索stage中提取最优的计划
CExpression * PexprExtractPlan(CMemoryPool *mp, CGroup *pgroupRoot,
                        CReqdPropPlan *prppInput, ULONG ulSearchStages)
```

