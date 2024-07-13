Executor执行需要执行下面几个借口来完成整个整个生命周期
1. `ExecutorStart()`
2. `ExecutorRun()`
3. `ExecutorFinish()`
4. `ExecutorEnd()`

PG对外提供了Hook用于插件控制执行流程
```c
ExecutorStart_hook_type ExecutorStart_hook = NULL;
ExecutorRun_hook_type ExecutorRun_hook = NULL;
ExecutorFinish_hook_type ExecutorFinish_hook = NULL;
ExecutorEnd_hook_type ExecutorEnd_hook = NULL;
```
如果用户未指定Hook，默认执行`standard_ExecutorXX`,例如`ExecutorStart`
```c
void ExecutorStart(QueryDesc *queryDesc, int eflags) {
  if (ExecutorStart_hook)
    (*ExecutorStart_hook) (queryDesc, eflags);
  else
    standard_ExecutorStart(queryDesc, eflags);
}
```

# 重点数据结构
## TargetEntry
`TargetEntry`用于表示targetlist中的元素。
```C
typedef struct TargetEntry {
  // 继承自Expr，表示这是一个表达式
  Expr    xpr;
  // 用于计算的表达式本身
  Expr     *expr;
  // 在结果集中的属性编号
  AttrNumber  resno;
  // 列属性名称，可以为NULL
  char     *resname pg_node_attr(query_jumble_ignore);
  // sort/group子句的引用
  Index    ressortgroupref;
  // source表的OID(用于查询结果元数据)
  Oid      resorigtbl pg_node_attr(query_jumble_ignore);
  // source表的字段编号
  AttrNumber  resorigcol pg_node_attr(query_jumble_ignore);
  // 用于标记查询执行过程不需要返回给客户端
  // 但在执行过程中需要使用的目标列表项
  bool    resjunk pg_node_attr(query_jumble_ignore);
} TargetEntry;
```

相关操作
```c
// 从targetlist中提取TupleDesc
TupleDesc ExecTypeFromTL(List *targetList)
```

```c
ExecutePlan
  slot = ExecProcNode(planstate);
    if (sendTuples) {
      if (!dest->receiveSlot(slot, dest)) break;
    }
```