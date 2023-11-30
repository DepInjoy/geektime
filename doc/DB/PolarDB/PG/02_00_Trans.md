```shell
// src/backend/utils/misc/postgresql.conf.sample.polardb_pg
synchronous_commit=on
```

```sql
-- 读为提交, 最低隔离级别,允许脏读
SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;
-- 读提交,不允许脏读
SET TRANSACTION ISOLATION LEVEL READ COMMITTED;
-- 可重复读
SET TRANSACTION ISOLATION LEVEL REPEATABLE READ;
-- 可串行化,最高隔离级别
SET TRANSACTION ISOLATION LEVEL SERIALIZABLE;
```

```sql
polar_csn_enable = true
```

```C++
src\backend\access\transam\twophase.c
src\backend\access\transam\twophase_rmgr.c
src\backend\access\transam\transam.c
```

```C++
bool							/* true if given transaction committed */
TransactionIdDidCommit(TransactionId transactionId) {
	XidStatus	xidstatus;
	if (polar_csn_enable) {
		return polar_xact_did_commit(transactionId);
	}

	xidstatus = TransactionLogFetch(transactionId);
    ......
}
```

```C++
src\backend\access\transam\polar_csnlog.c
src\backend\access\transam\polar_csn_mvcc_vars.c
src\backend\access\transam\xact.c
```



```C++
/*
 * This must be called ONCE during postmaster or standalone-backend startup,
 * after StartupXLOG has initialized ShmemVariableCache->nextXid.
 *
 * oldestActiveXID is the oldest XID of any prepared transaction, or nextXid
 * if there are none.
 */
void StartupSUBTRANS(TransactionId oldestActiveXID)
```

```C++
/*
 * This must be called ONCE during postmaster or standalone-backend startup
 */
// src\backend\access\transam\xlog.c
void StartupXLOG(void)
    
```

```C++
/*
 * POLAR: 
 * 	  Initialization for making polar dynamic bgworker visible 
 * 		in pg_stat_activity and polar_stat_activity
 *    Then how other backends implement this?
 *    	* client backends: InitPostgres()
 *    	* static bg worker: also InitPostgres()
 *    	* Auxiliary proc: in AuxiliaryProcessMain
 * 	  
 *    So we need to borrow codes from InitPostgres/AuxiliaryProcessMain and 
 * 		we'd better minimize this work for not involving additional initialization
 * 		and reducing the side effects.
 */
void
polar_init_dynamic_bgworker_in_backends(void)
```

#  参考资料

1. [知乎:Polardb中事务提交时间戳CSN源码详解](https://zhuanlan.zhihu.com/p/591628002)