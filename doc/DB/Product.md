| 数据库产品                                                   | 查询优化模型 | 集群架构         | 存储结构 | 扩展能力 | 计算能力 |
| ------------------------------------------------------------ | ------------ | ---------------- | -------- | -------- | -------- |
| GreenPlum                                                    |              |                  |          |          |          |
| TiDB是PingCAP公司研发的开源云原生的分布式HTAP关系型数据库，兼容 MySQL 5.7 协议和 MySQL 生态。适合于高可用、强一致、数据规模大等应用场景。<br/><br/>基于计算存储分离架构，可根据业务需求，对计算节点或者存储节点扩容。<br/>云原生分布式通过 TiDB Operator 在云上实现部署自动化<br/>提供行存储引擎 [TiKV](https://docs.pingcap.com/zh/tidb/stable/tikv-overview)、列存储引擎 [TiFlash](https://docs.pingcap.com/zh/tidb/stable/tiflash-overview) 两款存储引擎<br/>采用Mulit-Raft，能很好地保证数据分片的一致性<br/>TiSpark 组件为解决复杂 OLAP 需求，借助 Spark 平台，同时结合 TiKV以及TiFlash分布式行列混合集群优势，和 TiDB 一起为用户一站式解决 HTAP需求。TiSpark 依赖于 TiKV 集群和 Placement Driver (PD)，也需要搭建Spark 集群。<br/> | 向量化模型   |                  |          |          |          |
| OceanBase是蚂蚁集团研发的国产原生分布式数据库，云原生、多租户、强一致性、高度兼容Oracle/MySQL等特性。<br/><br/>1. OceanBase1.0版本实现了运算符融合<br/>2. OceanBase 2.0 版本中就实现了表达式代码生<br/> | 代码生成模型 | P2P对等模式<br/> |          |          |          |
| Kudu                                                         |              |                  |          |          |          |
| Impala                                                       |              |                  |          |          |          |
| ClickHouse                                                   | 向量化模型   |                  |          |          |          |
| TBase                                                        |              |                  |          |          |          |
| TDSQL                                                        |              |                  |          |          |          |
| CockroachDB                                                  | 向量化模型   |                  |          |          |          |
|                                                              | 代码生成模型 |                  |          |          |          |

