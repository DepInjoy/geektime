
```sql
-- 在当前事务中定义子事务(Savepoint)
SAVEPOINT savepoint_name;

-- 回滚到指定的子事务
ROLLBACK TO SAVEPOINT savepoint_name;

-- 释放之前定义的子事务,只是Savepoint被释放,中间的修改不会失效
RELEASE SAVEPOINT savepoint_name;
```

例如
```sql
BEGIN;
    INSERT INTO table1 VALUES (1);
    SAVEPOINT my_savepoint;
    INSERT INTO table1 VALUES (2);
    ROLLBACK TO SAVEPOINT my_savepoint;
    INSERT INTO table1 VALUES (3);
COMMIT;
-- 1,3被插入, 2不会被插入

BEGIN;
    INSERT INTO table1 VALUES (3);
    SAVEPOINT my_savepoint;
    INSERT INTO table1 VALUES (4);
    RELEASE SAVEPOINT my_savepoint;
COMMIT;
-- 3和4都被插入
```
在同一个事务中采用相同的子事务名。
```sql
BEGIN;
    INSERT INTO table1 VALUES (1);
    SAVEPOINT my_savepoint;
    INSERT INTO table1 VALUES (2);
    SAVEPOINT my_savepoint;
    INSERT INTO table1 VALUES (3);

    -- rollback to the second savepoint
    ROLLBACK TO SAVEPOINT my_savepoint;
    SELECT * FROM table1;               -- shows rows 1 and 2

    -- release the second savepoint
    RELEASE SAVEPOINT my_savepoint;

    -- rollback to the first savepoint
    ROLLBACK TO SAVEPOINT my_savepoint;
    SELECT * FROM table1;               -- shows only row 1
COMMIT;
```

PG的子事务是栈模式，每次新创建子事务就压栈进去。而如果当前事务中如果有多个子事务，则前一个子事务是它后面一个子事务的父事务（parent）；通过不断地压栈和出栈，修改事务块状态实现子事务的定义、回滚、提交。

# 子事务源码解读
```C++
// 子事务ID,用于设置检查点
typedef uint32 SubTransactionId;
#define InvalidSubTransactionId		((SubTransactionId) 0)
#define TopSubTransactionId			((SubTransactionId) 1)
```

```C++
// src/backend/access/transam/xact.c

// 子事务实现通过TransactionState的栈操作来实现
// 每创建一个子事务创建TransactionState并压栈(参见PushTransaction)
// 每个TransactionState都有一个指向父事务结构的指针(参见parent)
typedef struct TransactionStateData {
	FullTransactionId fullTransactionId;	/* my FullTransactionId */

	SubTransactionId subTransactionId;		// 子事务号
	char	   *name;                       // savepoint名称
	int			savepointLevel;             // 等于父事务的savepoint level

	TransState	state;			/* low-level state */
	TBlockState blockState;		/* high-level state */
	int			nestingLevel;	            // 事务嵌套深度, 等于父事务的nestingLevel+1
	int			gucNestLevel;	/* GUC context nesting depth */

    MemoryContext curTransactionContext;	/* my xact-lifetime context */
	ResourceOwner curTransactionOwner;	/* my query resources */
	TransactionId *childXids;	/* subcommitted child XIDs, in XID order */
	int			nChildXids;		/* # of subcommitted child XIDs */
	int			maxChildXids;	/* allocated size of childXids[] */
	Oid			prevUser;		/* previous CurrentUserId setting */
	int			prevSecContext; /* previous SecurityRestrictionContext */
	bool		prevXactReadOnly;	/* entry-time xact r/o state */
	bool		startedInRecovery;	/* did we start in recovery? */
	bool		didLogXid;		/* has xid been included in WAL record? */
	int			parallelModeLevel;	/* Enter/ExitParallelMode counter */
	bool		chain;			/* start a new block after this one */
	bool		assigned;		/* assigned to top-level XID */

	struct TransactionStateData *parent;			// 父事务状态指针
} TransactionStateData;

typedef TransactionStateData *TransactionState;
```

```C++
// 如果当前没有在执行事务, CurrentTransactionState指向TopTransactionStateData
static TransactionStateData TopTransactionStateData = {
	.state = TRANS_DEFAULT,
	.blockState = TBLOCK_DEFAULT,
	.assigned = false,
};
static TransactionState CurrentTransactionState = &TopTransactionStateData;

// 当前子事务号,每创建一个子事务(PushTransaction)自增1
static SubTransactionId currentSubTransactionId;

static void StartTransaction(void) {
  // 开始执行第一条SQL之前，假设state stack是空
	s = &TopTransactionStateData;
	CurrentTransactionState = s;
  		......
}
```


## 定义子事务

```C++
// src/backend/access/transam/xact.c
void DefineSavepoint(const char *name) {
    TransactionState s = CurrentTransactionState;
    if (IsInParallelMode())
    ereport(ERROR, (errcode(ERRCODE_INVALID_TRANSACTION_STATE),
        errmsg("cannot define savepoints during a parallel operation")));
    
    switch (s->blockState) {
		case TBLOCK_INPROGRESS:
		case TBLOCK_SUBINPROGRESS:
            // 开启子事务，主逻辑
			PushTransaction();
			s = CurrentTransactionState;
			if (name)
				s->name = MemoryContextStrdup(TopTransactionContext, name);
			break;

        ...... // eror,省略
    }
}
```

```C++
// 为子事务创建TransactionState
static void PushTransaction(void) {
	TransactionState p = CurrentTransactionState;
	TransactionState s;
	s = (TransactionState) MemoryContextAllocZero(TopTransactionContext,
			sizeof(TransactionStateData));
  		.......
  	
	// 分配子事务号
  	currentSubTransactionId += 1;
 
  	s->fullTransactionId = InvalidFullTransactionId;
	s->subTransactionId = currentSubTransactionId;
	// 设置父事务(当前TransactionState)
	s->parent = p;

  	s->nestingLevel = p->nestingLevel + 1;
	s->gucNestLevel = NewGUCNestLevel();
    // 子事务的savepointLevel和父事务的savepointLevel是相同的
	s->savepointLevel = p->savepointLevel;
	s->state = TRANS_DEFAULT;
	// 设置blockState为开启子事务
	s->blockState = TBLOCK_SUBBEGIN;
	GetUserIdAndSecContext(&s->prevUser, &s->prevSecContext);
	s->prevXactReadOnly = XactReadOnly;
	s->parallelModeLevel = 0;
	s->assigned = false;

	CurrentTransactionState = s;
}
```

## 回滚到指定子事务

```C++
// 回滚到指定savepoint, 即ROLLBACK TO <savepoint>
void RollbackToSavepoint(const char *name) {
    TransactionState s = CurrentTransactionState;
	TransactionState target, xact;
    switch (s->blockState) {
        case TBLOCK_SUBINPROGRESS:
		case TBLOCK_SUBABORT:
			break;
        ...... // 其他报错，忽略
    }

    // 1. 根据子事务名称,查找相应的TransactionState
    for (target = s; PointerIsValid(target); target = target->parent) {
		if (PointerIsValid(target->name) && strcmp(target->name, name) == 0)
			break;
	}

    // 子事务的savepointLevel和父事务的应该相同(见PushTransaction), 如果不同则报错
	if (target->savepointLevel != s->savepointLevel)
		ereport(ERROR, (errcode(ERRCODE_S_E_INVALID_SPECIFICATION),
            errmsg("savepoint \"%s\" does not exist within current savepoint level", name)));

    // 2. 从当前TransactionState开始,逐级给blockState设置状态直到target
	xact = CurrentTransactionState;
	for (;;) {
        // 回滚到target,退出循环
		if (xact == target) break; 
		if (xact->blockState == TBLOCK_SUBINPROGRESS)
			xact->blockState = TBLOCK_SUBABORT_PENDING;
		else if (xact->blockState == TBLOCK_SUBABORT)
			xact->blockState = TBLOCK_SUBABORT_END;
		else 
			elog(FATAL, "RollbackToSavepoint: unexpected state %s",
				 BlockStateAsString(xact->blockState));
        // 回退到父事务
		xact = xact->parent;
	}

    // 3. 将target的blockState设置为restart或pengding
	if (xact->blockState == TBLOCK_SUBINPROGRESS)
		xact->blockState = TBLOCK_SUBRESTART;
	else if (xact->blockState == TBLOCK_SUBABORT)
		xact->blockState = TBLOCK_SUBABORT_RESTART;
	else
		elog(FATAL, "RollbackToSavepoint: unexpected state %s",
			 BlockStateAsString(xact->blockState));
}
```

## 释放子事务

```C++
void ReleaseSavepoint(const char *name) {
	TransactionState s = CurrentTransactionState;
	TransactionState target, xact;
	switch (s->blockState) {
        case TBLOCK_SUBINPROGRESS:
        break;
        ...... // 其他报错，忽略
    }

    // 1. 根据子事务名称,查找相应的TransactionState
    for (target = s; PointerIsValid(target); target = target->parent) {
		if (PointerIsValid(target->name) && strcmp(target->name, name) == 0)
			break;
	}

    // 2. 从当前TransactionState开始,逐级给blockState设置状态直到target
    xact = CurrentTransactionState;
	for (;;) {
		xact->blockState = TBLOCK_SUBRELEASE;
		if (xact == target) break;
		xact = xact->parent;
	}
}
```

# 参考资料

1. [PostgreSQL保存点/子事务(SAVEPOINT)](https://blog.csdn.net/u013970710/article/details/127150157#:~:text=%E6%80%BB%E4%BD%93%E6%9D%A5%E8%AF%B4%EF%BC%8CPG%E7%9A%84,%E5%AE%9A%E4%B9%89%E3%80%81%E5%9B%9E%E6%BB%9A%E3%80%81%E6%8F%90%E4%BA%A4%E3%80%82)
2. [Postgresql源码（25）子事务可见性判断和性能问题](https://blog.csdn.net/jackgo73/article/details/122363386)
3. [Postgresql源码（71）子事务数据结构与DDL](https://blog.csdn.net/jackgo73/article/details/126366335)
4. [Postgrs doc: SAVEPOINT](https://www.postgresql.org/docs/current/sql-savepoint.html)

