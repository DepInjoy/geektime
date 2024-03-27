# 架构
GreenPlum采用无共享(Share-Nothing)的MPP(Massively Parallel Processing)架构。在Greenplum数据库中，每个处理单元叫作Segment节点。负责生成查询计划并协调每个处理单元进行查询执行的节点叫作Master节点。除了Master和Segment节点外，有一个重要的功能组件叫作Interconnect，主要负责在查询执行过程中，实现节点间的数据传输。同时，为了保障Greenplum架构的高可用，Master节点和每个Segment节点都有自己的备份节点，分别称为StandbyMaster节点和Mirror Segment节点，为Master节点和Segment节点提供高可用支持。Greenplum的高可用架构之所以采用主备方式而不是类似Hadoop的多副本机制，主要是因为Hadoop的磁盘存储是基于磁盘簇（Just Bundle Of Disks，JBOD），每一块磁盘没有冗余机制，而是通过应用层的多副本来保证数据可靠性。Greenplum建议的磁盘存储方式是RAID（Redundant Array Of InexpensiveDisks），每一块磁盘在物理上都有冗余机制，主备方式则是在磁盘冗余基础上提供另一级的数据可靠性。

# 运行视图

<table>
    <tr><td bgcolor=white>
        <img src=https://static001.infoq.cn/resource/image/9f/b1/9f944f1094ae71d857bbebabe8c931b1.png>
    </td></tr>
</table>



QD(Query Dispatcher、查询调度器)：Master 节点上负责处理用户查询请求的进程称为 QD(PostgreSQL 中称之为 Backend 进程)。 QD 收到用户发来的 SQL 请求后，进行解析、重写和优化，将优化后的并行计划分发给每个 segment 上执行，并将最终结果返回给用户。此外还负责整个 SQL 语句涉及到的所有的 QE 进程间的通讯控制和协调，譬如某个 QE 执行时出现错误时，QD 负责收集错误详细信息，并取消所有其他 QEs。QD 的入口是`exec_simple_query()`。
```C
// src/backend/tcop/postgres.c
static void exec_simple_query(const char *query_string)
```

QE(Query Executor、查询执行器)：Segment 上负责执行 QD 分发来的查询任务的进程称为 QE。Segment 实例运行的也是一个 PostgreSQL，所以对于 QE 而言，QD 是一个 PostgreSQL 的客户端，通过 PostgreSQL 标准的 libpq 协议进行通讯(GP对其做了增强)。对于 QD 而言，QE 是负责执行查询请求的PostgreSQL Backend 进程。通常 QE 执行整个查询的一部分(称为 Slice)。QE 的入口是`exec_mpp_query()`。
```C
// src/backend/tcop/postgres.c
static void exec_mpp_query(const char *query_string,
			   const char * serializedQuerytree, int serializedQuerytreelen,
			   const char * serializedPlantree, int serializedPlantreelen,
			   const char * serializedParams, int serializedParamslen,
			   const char * serializedQueryDispatchDesc, int serializedQueryDispatchDesclen)
```

Slice：为了提高查询执行并行度和效率，Greenplum 把一个完整的分布式查询计划从下到上分成多个 Slice，每个 Slice 负责计划的一部分。划分 slice 的边界为 Motion，每遇到 Motion 则将Motion切成发送方和接收方，得到两颗子树。每个slice由一个QE进程处理。上面例子中一共有三个 slice。

Gang：在不同 segments 上执行同一个 slice 的所有 QEs 进程称为 Gang。上例中有两组 Gang，第一组 Gang 负责在 2 个 segments 上分别对表 classes 顺序扫描，并把结果数据重分布发送给第二组 Gang；第二组 Gang 在 2 个 segments 上分别对表 students 顺序扫描，与第一组 Gang 发送到本 segment 的 classes 数据进行哈希关联，并将最终结果发送给 Master。



## 并行执行流程

假设有 2 个 segments，查询计划有两个 slices，一共有 4 个 QEs，查询在Grenplum中的并行执行流程。

<table>
    <tr><td bgcolor=white>
        <img src=https://static001.infoq.cn/resource/image/1d/ad/1d86c3d16d0c3e7fd09d936ee9fd7aad.png>
    </td></tr>
</table>

QD 和 QE 都是 PostgreSQL backend 进程，其执行逻辑非常相似。对于数据操作(DML)语句，其核心执行逻辑由 ExecutorStart, ExecutorRun, ExecutorEnd 实现。



QD的核心执行逻辑：

- ExecutorStart 负责执行器的初始化和启动。Greenplum 通过 CdbDispatchPlan 把完整的查询计划发送给每个 Gang 中的每个 QE 进程。Greenplum 有两种发送计划给 QE 的方式：
  1. 异步方式，使用 libpq 的异步 API 以非阻塞方式发送查询计划给 QE，从6.0版本开始去掉了该方式。
  2. 同步多线程方式：使用 libpq 的同步 API，使用多个线程同时发送查询计划给 QE。GUC gp_connections_per_thread 控制使用线程数量，缺省值为 0，表示采用异步方式。
- ExecutorRun启动执行器，执行查询树中每个算子的代码，并以火山模型（volcano）风格返回结果元组给客户端。在 QD 上，ExecutorRun调用 ExecutePlan 处理查询树，该查询树的最下端的节点是一个 Motion 算子。其对应的函数为 ExecMotion，该函数等待来自于各个 QE 的结果。QD 获得来自于 QE 的元组后，执行某些必要操作（譬如排序）然后返回给最终用户。
- ExecutorEnd负责执行器的清理工作，包括检查结果，关闭 interconnect 连接等。



QE 上的 ExecutorStart/ExecutorRun/ExecutorEnd 函数和单节点的 PostgreSQL 代码逻辑类似。主要的区别在 QE 执行的是 Greenplum 分布式计划中的一个 slice，因而其查询树的根节点一定是个 Motion 节点。其对应的执行函数为 ExecMotion，该算子从查询树下部获得元组，并根据 Motion 的类型发送给不同的接收方。低一级的 Gang 的 QE 把 Motion 节点的结果元组发送给上一级 Gang 的 QE，最顶层 Gang 的 QE 的 Motion 会把结果元组发送给 QD。Motion 的 Flow 类型确定了数据传输的方式，有两种数据传输方式：

- 广播，将数据发送给上一级 Gang 的每一个 QE
- 重分布，将数据根据重分布键计算其对应的 QE 处理节点，并发送给该 QE。



QD 和 QE 之间有两种类型的网络连接：

- libpq：QD 通过 libpq 与各个 QE 间传输控制信息，包括发送查询计划、收集错误信息、处理取消操作等。libpq 是 PostgreSQL 的标准协议，Greenplum 对该协议进行了增强，譬如新增了‘M’消息类型（QD 使用该消息发送查询计划给 QE）。libpq 是基于 TCP 的。
- interconnect：QD 和 QE、QE 和 QE 之间的表元组数据传输通过 interconnect 实现。 Greenplum 有两种 interconnect 实现方式，一种基于 TCP，一种基于 UDP。默认采取UDP interconnect连接方式。

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



# 参考资料

**很棒的文章**
- [Greenplum：基于 PostgreSQL 的分布式数据库内核揭秘 (下篇)](https://www.infoq.cn/article/iadfebtb1y0mojlvrscu)

