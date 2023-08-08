PG采用栈模式实现子事务，每个事务都有一个`TransactionStateData`，开启事务时，当前事务状态指向`TopTransactionStateData`(参见`StartTransaction`)。

```C++
// 如果当前没有在执行事务, CurrentTransactionState指向TopTransactionStateData
static TransactionStateData TopTransactionStateData = {
	.state = TRANS_DEFAULT,
	.blockState = TBLOCK_DEFAULT,
	.assigned = false,
};
static TransactionState CurrentTransactionState = &TopTransactionStateData;
```





```C++
// 中间层
void StartTransactionCommand(void) {
	TransactionState s = CurrentTransactionState;
	switch (s->blockState) {
         // idle,开启事务
		case TBLOCK_DEFAULT:
			StartTransaction();
			s->blockState = TBLOCK_STARTED;
			break;
		case TBLOCK_INPROGRESS:
		case TBLOCK_IMPLICIT_INPROGRESS:
		case TBLOCK_SUBINPROGRESS:
			break;
		case TBLOCK_ABORT:
		case TBLOCK_SUBABORT:
			break;

		......	// 其他报错
			break;
	}

	MemoryContextSwitchTo(CurTransactionContext);
}
```



```C++
static void StartTransaction(void) {
	TransactionState s;
	VirtualTransactionId vxid;
    // 开始执行第一条SQL之前，假设state stack是空
	s = &TopTransactionStateData;
    // 当前事务状态取顶层事务状态
	CurrentTransactionState = s;

    // 事务状态设置为TRANS_START
    s->state = TRANS_START;
	s->fullTransactionId = InvalidFullTransactionId;
    
    s->nestingLevel = 1;
	s->gucNestLevel = 1;
	s->childXids = NULL;
	s->nChildXids = 0;
	s->maxChildXids = 0;
    
    
    	.......

    s->didLogXid = false;
    
    // 为TopTransactionContext申请内存
    // CurTransactionContext和s->curTransactionContext
    // 都指向TopTransactionContext
	AtStart_Memory();
    
    // 创建TopTransaction Resource Owner且s->curTransactionOwner指向它
	AtStart_ResourceOwner();
    
    // 
    vxid.backendId = MyBackendId;
	vxid.localTransactionId = GetNextLocalTransactionId();
	VirtualXactLockTableInsert(vxid);
    
    // 事务状态设置为TRANS_INPROGRESS
    s->state = TRANS_INPROGRESS;
}
```



```C++
// 回到父事务
static void PopTransaction(void) {
  TransactionState s = CurrentTransactionState;
  CurrentTransactionState = s->parent;
  // 设置当前事务memory context
  CurTransactionContext = s->parent->curTransactionContext;
  CurTransactionResourceOwner = s->parent->curTransactionOwner;
  CurrentResourceOwner = s->parent->curTransactionOwner;
  			......
}
```

