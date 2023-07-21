PolarDB for PostgreSQL是一款阿里云自主研发的云原生关系型数据库产品，<b>100%兼容 PostgreSQL，高度兼容Oracle语法；采用基于 Shared-Storage 的存储计算分离架构</b>，具有极致弹性、毫秒级延迟、HTAP 的能力和高可靠、高可用、弹性扩展等企业级数据库特性。同时，PolarDB-PG具有大规模并行计算能力，可以应对OLTP与OLAP混合负载。
- 极致弹性：存储与计算能力均可独立地横向扩展。
    - 当计算能力不够时，可以单独扩展计算集群，数据无需复制。
    - 当存储容量或I/O不够时，可以单独扩展存储集群，而不中断业务。

- 毫秒级延迟：
    - WAL日志存储在共享存储上，RW到所有RO之间仅复制WAL的元数据。
    - 独创的LogIndex技术，实现了Lazy回放和Parallel回放，理论上最大程度地缩小了RW和RO节点间的延迟。
- HTAP 能力：基于Shared-Storage的分布式并行执行框架，加速在OLTP场景下的OLAP查询。一套OLTP型的数据，可支持2套计算引擎：
    - 单机执行引擎：处理高并发的 TP 型负载。
    - 分布式执行引擎：处理大查询的 AP 型负载。

PolarDB-PG还支持时空、GIS、图像、向量、搜索、图谱等多模创新特性。

PolarDB-PG支持多种部署形态：单机本地部署、存储计算分离部署、X-Paxos三节点部署。


# 参考资料
1. [PolarDB for PostgreSQL简介](https://docs.polardbpg.com/1653230754878/Introduction.html)