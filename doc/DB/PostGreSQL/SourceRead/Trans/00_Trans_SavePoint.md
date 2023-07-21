
```C++
// src/backend/access/transam/xact.c
// 执行定义savepoint,主要调用PushTransaction()接口
void DefineSavepoint(const char *name) {

}
```



```C++
// 将当前事务回滚到指定savepoint
void CommitTransactionCommand(void) {}

// 执行回滚到指定savepoint, 即ROLLBACK TO <savepoint>
void RollbackToSavepoint(const char *name) {
    
}
```

# 参考资料

1. [PostgreSQL保存点/子事务(SAVEPOINT)](https://blog.csdn.net/u013970710/article/details/127150157#:~:text=%E6%80%BB%E4%BD%93%E6%9D%A5%E8%AF%B4%EF%BC%8CPG%E7%9A%84,%E5%AE%9A%E4%B9%89%E3%80%81%E5%9B%9E%E6%BB%9A%E3%80%81%E6%8F%90%E4%BA%A4%E3%80%82)
2. [Postgresql源码（25）子事务可见性判断和性能问题](https://blog.csdn.net/jackgo73/article/details/122363386)
3. [Postgresql源码（71）子事务数据结构与DDL](https://blog.csdn.net/jackgo73/article/details/126366335)

