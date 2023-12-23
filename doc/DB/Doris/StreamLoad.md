Stream load是一个同步的导入方式，用户通过发送HTTP协议发送请求将本地文件或数据流导入到Doris中。Stream load主要适用于导入本地文件，或通过程序导入数据流中的数据。

# Stream Load的实现

> Doris Stream Load 实现的核心思想和HBase，Druid比较类似：在内存中实现一个面向写优化的 MemTable(MemStore)，当MemTable的大小超过一定阈值时，进行flush，将MemTable转为带有前缀索引的SSTable(HBase 中是将MemStore转为HFile, Druid中是将Map转为带倒排索引的 Segment）
>
> Doris新增了GlobalTransactionMgr 来保证单次Stream Load 的原子性[1]。

# 参考资料

1. [Apache Doris Stream Load & Kakfa To Doris](https://blog.bcmeng.com/post/kafka-to-doris.html)