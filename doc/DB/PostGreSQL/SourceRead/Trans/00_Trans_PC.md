```C++
// 执行PREPARE命令
bool PrepareTransactionBlock(const char *gid)
```

```C
// 执行COMMIT命令
bool EndTransactionBlock(bool chain)
```

```c
// 执行ROLLBACK命令
void UserAbortTransactionBlock(bool chain)
```

```C++
/*
 * Start preparing a state file.
 *
 * Initializes data structure and inserts the 2PC file header record.
 */
void StartPrepare(GlobalTransaction gxact)
    
/*
 * Finish preparing state data and writing it to WAL.
 */
void EndPrepare(GlobalTransaction gxact)
```

# 参考资料

1. [PostgreSQL两阶段提交事务源码分析](https://www.mytecdb.com/blogDetail.php?id=246)
