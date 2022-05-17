# 架构
GreenPlum采用无共享(Share-Nothing)的MPP(Massively Parallel Processing)架构。在Greenplum数据库中，每个处理单元叫作Segment节点。负责生成查询计划并协调每个处理单元进行查询执行的节点叫作Master节点。除了Master和Segment节点外，有一个重要的功能组件叫作Interconnect，主要负责在查询执行过程中，实现节点间的数据传输。同时，为了保障Greenplum架构的高可用，Master节点和每个Segment节点都有自己的备份节点，分别称为StandbyMaster节点和Mirror Segment节点，为Master节点和Segment节点提供高可用支持。Greenplum的高可用架构之所以采用主备方式而不是类似Hadoop的多副本机制，主要是因为Hadoop的磁盘存储是基于磁盘簇（Just Bundle Of Disks，JBOD），每一块磁盘没有冗余机制，而是通过应用层的多副本来保证数据可靠性。Greenplum建议的磁盘存储方式是RAID（Redundant Array Of InexpensiveDisks），每一块磁盘在物理上都有冗余机制，主备方式则是在磁盘冗余基础上提供另一级的数据可靠性。

# 查询处理过程

## 主要功能组件
1. 解析器
> Master节点接收到客户端请求后，进行认证操作。认证成功并建立连接后，客户端发送查询语句给数据库。解析器负责对收到的查询语句，即SQL字符串进行词法分析、语法分析，并生成查询树。

2. 优化器
> 查询计划描述了如何执行一条查询，通常以树形结构表示。查询优化器用于对解析器的结果（即查询树）进行处理，从所有可能的查询计划中选择一个最优的或者接近最优的查询计划，作为最终的查询计划。Greenplum数据库支持两种优化器，一种是基于PostgreSQL的planner查询优化器，另一种是全新的ORCA查询优化器。ORCA是Pivotal开源的针对OLAP业务的查询优化器，它在模块化、扩展性、性能等很多方面相比planner查询优化器有很大的改进，性能提升几十倍到上千倍不等。

3. 调度器
> 
> 根据查询计划中Slice的定义，调度器将查询计划发送给所有Segment节点上的执行器，待执行器执行完查询任务后，收集所有执行器的查询结果，汇总后返回给客户端。调度器在查询执行过程中，主要负责执行器的创建、销毁、错误处理、任务取消、状态更新等。QD 的入口是 exec_simple_query()。

4. 执行器
> 当执行器收到调度器发送的查询计划后，就执行查询计划中自己所负责的那个部分。典型的操作包括表扫描、哈希关联、排序、聚集等。QE 的入口是 exec_mpp_query()。

1. InterConnect
> Interconnect主要负责集群中各个节点间的数据传输。

6. 系统表
> 系统表用于存储和管理数据库、表、字段的元数据。每个节点上都有相应的数据拷贝。

7. 分布式事务
> Master节点上的分布式事务管理器负责协调Segment数据节点上的事务提交和回滚操作，由两阶段提交（2PC）协议实现。每个Segment数据节点都有自己的事务日志，负责自己节点上的事务处理操作。

## 查询执行流程
![](./img/Quey-Execute-Flow.jpg)
> Greenplum数据库查询的执行主要有以下步骤：
> 1）客户端（如PSQL）通过libpq协议首先连接到Master节点的Postmaster进程。
> 2）Master节点的Postmaster进程产生一个后端Postgres进程QD，QD与客户端进行连接，执行客户端请求认证。认证通过后，QD等待客户端发送查询请求。
> 3）客户端将查询请求提交给QD进程。
> 4）QD生成查询计划，并根据查询计划中的Slice数量，通过libpq与每个Segment实例上的Postmaster进程连接以启动QE进行查询执行。每个Segment实例上启动的QE数量等于需要执行的Slice数量。
> 5）QD将查询计划分发给所有Segment实例上所有的QE进程。
> 6）每个QE进程根据查询计划执行属于自己的Slice，不同Slice间QE的数据通信通过Interconnect完成。
> 7）所有负责执行最后一个Slice的QE将查询执行的结果返回给QD。
> 8）QD汇总查询结果，返回给客户端，并等待下一个查询语句的到来。

# 参考资料

**很棒的文章**
- [Greenplum：基于 PostgreSQL 的分布式数据库内核揭秘 (下篇)](https://www.infoq.cn/article/iadfebtb1y0mojlvrscu)

