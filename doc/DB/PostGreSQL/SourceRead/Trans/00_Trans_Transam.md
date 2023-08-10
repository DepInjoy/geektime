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

PG在执行每个查询之前在中间层都会先执行`StartTransactionCommand`的指令，查询执行完成还会调用`CommitTransactionCommand`接口。

```
例如，按照下面的顺序执行用户指令：

1)		BEGIN
2)		SELECT * FROM foo
3)		INSERT INTO foo VALUES (...)
4)		COMMIT

PG处理过程中实际调用的接口
     /  StartTransactionCommand;
    /       StartTransaction;
1) <    ProcessUtility;                 << BEGIN
    \       BeginTransactionBlock;
     \  CommitTransactionCommand;

    /   StartTransactionCommand;
2) /    PortalRunSelect;                << SELECT ...
   \    CommitTransactionCommand;
    \       CommandCounterIncrement;

    /   StartTransactionCommand;
3) /    ProcessQuery;                   << INSERT ...
   \    CommitTransactionCommand;
    \       CommandCounterIncrement;

     /  StartTransactionCommand;
    /   ProcessUtility;                 << COMMIT
4) <        EndTransactionBlock;
    \   CommitTransactionCommand;
     \      CommitTransaction;
```

# 中间层事务接口

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

# 顶层事务接口

PG提供的事务操作的SQL也是命令的一种，下面的事务操作和PG的API的对应关系

| 用户层事务指令 | 实现接口                                     |
| -------------- | -------------------------------------------- |
| `BEGIN`        | `void BeginTransactionBlock(void)`           |
| `COMMIT`       | `bool EndTransactionBlock(bool chain)`       |
| `ROLL BACK`    | `void UserAbortTransactionBlock(bool chain)` |

在具体的实现中，这些接口主要是设置事务状态(`TransactionState`)中`blockState`的值，来通知`CommitTransactionCommand`，实际的执行动作由`CommitTransactionCommand`执行中，会调用一些事务相关的底层接口。



| 用户层子事务指令             | 实现接口                                     |
| :--------------------------- | -------------------------------------------- |
| `SAVEPOINT savepoint_name`   | `void DefineSavepoint(const char *name)`     |
| `ROLLBACK TO savepoint_name` | `void RollbackToSavepoint(const char *name)` |
| `RELEASE savepoit_name`      | `void ReleaseSavepoint(const char *name)`    |

子事务执行完用户命令对应的接口也会给事务状态(`TransactionState`)中`blockState`设置相应的状态值，之也会调用`CommitTransactionCommand`执行一些子事务底层的接口。

## 开启事务

```C++
/**
 * 	执行BEGIN命令
 * 	主要设置blockState的状态,实际的执行由CommitTransactionCommand负责
*/
void BeginTransactionBlock(void) {
	TransactionState s = CurrentTransactionState;
	switch (s->blockState) {
		// 执行语句(含BEGIN)PG的中间层会先StartTransactionCommand
		// 它会将blockState状态设置为TBLOCK_STARTED
		// blockState设置为TBLOCK_BEGIN,将一个隐式的事务转化为正常事务
		// 通知CommitTransactionCommand开始事务
		case TBLOCK_STARTED:
			s->blockState = TBLOCK_BEGIN;
			break;

	    // blockState设置为TBLOCK_BEGIN,将一个隐式的事务转化为正常事务
		// 通知CommitTransactionCommand开始事务
		case TBLOCK_IMPLICIT_INPROGRESS:
			s->blockState = TBLOCK_BEGIN;
			break;
	}
	// 其他,一些异常报错处理,忽略
}
```



## 提交事务

```C++
/**
 *  执行COMMIT指令
 *  主要实现设置相应的blockState的值，实际的执行由CommitTransactionCommand
 *
 *	@return true 实际上执行COMMIT，FALSE执行ROLL BACK	
*/
bool EndTransactionBlock(bool chain) {
	TransactionState s = CurrentTransactionState;
	bool result = false;
	switch (s->blockState) {
		// 将blockState设置为TBLOCK_END
		// 通知CommitTransactionCommand执行COMMIT
		case TBLOCK_INPROGRESS:
			s->blockState = TBLOCK_END;
			result = true;
			break;

		// failed事务,设置blockState为TBLOCK_ABORT_END
		// 通知CommitTransactionCommand执行ROLL BACK来退出
		case TBLOCK_ABORT:
			s->blockState = TBLOCK_ABORT_END;
			break;

		// 当前在活跃的自事务中,subcommit所有的父事务和主事务
		case TBLOCK_SUBINPROGRESS:
			while (s->parent != NULL) {
				// 提交所有的子事务
				if (s->blockState == TBLOCK_SUBINPROGRESS)
					s->blockState = TBLOCK_SUBCOMMIT;
				s = s->parent;
			}
			// 提交主事务
			if (s->blockState == TBLOCK_INPROGRESS)
				s->blockState = TBLOCK_END;
			result = true;
			break;

		// failed子事务，将主事务和该子事务的父事务都ROLL BACK
		case TBLOCK_SUBABORT:
			while (s->parent != NULL) {
				if (s->blockState == TBLOCK_SUBINPROGRESS)
					s->blockState = TBLOCK_SUBABORT_PENDING;
				else if (s->blockState == TBLOCK_SUBABORT)
					s->blockState = TBLOCK_SUBABORT_END;
				s = s->parent;
			}
			if (s->blockState == TBLOCK_INPROGRESS)
				s->blockState = TBLOCK_ABORT_PENDING;
			else if (s->blockState == TBLOCK_ABORT)
				s->blockState = TBLOCK_ABORT_END;
			break;
		
		// 其他，一些错误的判断,忽略
	}

	s->chain = chain;
	return result;
}
```

## 回滚事务

```C++
// 执行ROLL BACK指令
void UserAbortTransactionBlock(bool chain) {
	TransactionState s = CurrentTransactionState;
	switch (s->blockState) {
		// 事务进行中,设置TBLOCK_ABORT_PENDING
		// 通知CommitTransactionCommand终止并退出事务
		case TBLOCK_INPROGRESS:
			s->blockState = TBLOCK_ABORT_PENDING;
			break;

		// failed事务,设置blockState状态为TBLOCK_ABORT_END
		// 通知CommitTransactionCommand清理并回到idle状态(TBLOCK_DEFAULT)
		case TBLOCK_ABORT:
			s->blockState = TBLOCK_ABORT_END;
			break;

		// 如果当前在子事务中,终止其父子事务以及主事务
		case TBLOCK_SUBINPROGRESS:
		case TBLOCK_SUBABORT:
			// 终止所有的父子事务
			while (s->parent != NULL) {
				if (s->blockState == TBLOCK_SUBINPROGRESS)
					s->blockState = TBLOCK_SUBABORT_PENDING;
				else if (s->blockState == TBLOCK_SUBABORT)
					s->blockState = TBLOCK_SUBABORT_END;
				s = s->parent; // 出栈
			}
			// 终止主事务
			if (s->blockState == TBLOCK_INPROGRESS)
				s->blockState = TBLOCK_ABORT_PENDING;
			else if (s->blockState == TBLOCK_ABORT)
				s->blockState = TBLOCK_ABORT_END;
			break;

		case TBLOCK_STARTED:
		case TBLOCK_IMPLICIT_INPROGRESS:
			if (chain) // error报错,返回
			else // warning,之后
			// ROLLBACK AND CHAIN	  :	error
			// ROLLBACK without CHAIN : warning
			s->blockState = TBLOCK_ABORT_PENDING;
			break;
		// ...... 其他报错,忽略
	}
	s->chain = chain;
}
```



