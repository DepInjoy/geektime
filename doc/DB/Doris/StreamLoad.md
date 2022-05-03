# Stream Load的实现

> Doris Stream Load 实现的核心思想和 HBase，Druid 比较类似：在内存中实现一个面向写优化的 MemTable(MemStore)，当 MemTable 的大小超过一定阈值时，进行 flush，将 MemTable 转为带有前缀索引的 SSTable（HBase 中是将 MemStore 转为 HFile, Druid 中是将 Map 转为带倒排索引的 Segment）
>
> Doris 新增了 GlobalTransactionMgr 来保证单次 Stream Load 的原子性[1]。

# 参考资料

1. [Apache Doris Stream Load & Kakfa To Doris](https://blog.bcmeng.com/post/kafka-to-doris.html)