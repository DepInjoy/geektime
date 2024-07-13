```c
// 构建PartitionPruneInfo用于Executor进行分区裁剪
PartitionPruneInfo * make_partition_pruneinfo(
    PlannerInfo *root, RelOptInfo *parentrel,
    List *subpaths, List *prunequal)
```