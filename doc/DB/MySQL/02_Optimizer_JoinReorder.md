MySQL 提供了两个系统变量，用于控制优化器的优化程度：

- `optimizer_prune_level`， 基于返回行数的评估忽略某些执行计划，这种启发式的方法可以极大地减少优化时间而且很少丢失最佳计划。因此，该参数的默认设置为 1；如果确认优化器错过了最佳计划，可以将该参数设置为 0，不过这样可能导致优化时间的增加。
- `optimizer_search_depth`，优化器查找的深度。如果该参数大于查询中表的数量，可以得到更好的执行计划，但是优化时间更长；如果小于表的数量，可以更快完成优化，但可能获得的不是最优计划。例如，对于 12、13 个或者更多表的连接查询，如果将该参数设置为表的个数，可能需要几小时或者几天时间才能完成优化；如果将该参数修改为 3 或者 4，优化时间可能少于 1 分钟。该参数的默认值为 62；如果不确定是否合适，可以将其设置为 0，让优化器自动决定搜索的深度。

```C++
class Optimize_table_order {
  const uint search_depth;
  const uint prune_level;
};
```

 # 参考资料

1. [知乎：MySQL 优化器原来是这样工作的](https://zhuanlan.zhihu.com/p/192707721)