Flink可以成为主流的实时数据处理框架源于：
1. Flink及时吸收和采用了Google开源论文提到的DataFlow/Beam编程思想
2. 实现轻量且高效的分布式异步快照算法实现，为端到端的数据一致性提供了强大保证。该分布式异步快照算法的原型是Chandy-Lamport算法，来自Chandy和Lamport的论文分布式快照：确定分布式系统的全局状态。

# Flink集群
Flink在运行时包含两种进程：
- JobManager（又称为 JobMaster）：它具有与协调Flink应用程序分布式执行相关的职责：协调Task的分布式执行，包括调度Task、协调Checkpoint以及当Job failover时协调各个Task从 Checkpoint恢复等。该进程由三个组件组成：
    - ResourceManger：负责 Flink 集群中的资源提供、回收、分配-它管理task slots，Flink 为不同的环境和资源提供者（例如 YARN、Kubernetes 和 standalone 部署）实现了对应的ResourceManager.
    - Dispatcher: 提供了一个REST接口来提交Flink应用程序执行，并为每个提交的作业启动一个新的JobMaster; 还运行Flink WebUI用来提供作业执行信息。
    - JobMaster：负责管理单个JobGraph的执行，Flink集群中可以同时运行多个作业，每个作业都有自己的JobMaster。

- TaskManager（又称为 Worker）：执行Dataflow中的Tasks，包括内存Buffer的分配、Data Stream的传递等。TaskManager中的最小资源分配单位时Task Slot，TaskManager中Task Slot的数量表示并发处理的Task数量。需要注意的是，一个Task Slot中可以执行多个Operator。
<table><tr>
    <td bgcolor=white>
    <img src="./img/Flink-Arch.svg"></td>
</tr></table>
<center>
    <div>事件时间与处理时间关系</div>
</center>
<br/>

## Task和算子链
Task是Flink中资源调度的最小单位。
<table><tr>
    <td bgcolor=white>
    <img src="./img/tasks_chains.svg"></td>
</tr></table>
<center>
    <div>事件时间与处理时间关系</div>
</center>
<br/>

## Task Slot和资源

# 编程思想
## DataFlow模型的设计思想
DataFlow能够对无界、无序的数据源按数据本身的特征进行窗口计算，得到基于事件发生时间的有序结果，并能在准确性、延迟程度和处理成本之间取得平衡。
## 有界、无界与流处理、批处理
<center>
    <img src="./img/BoundAndUnboundDataSet.png">
    <div>有界数据集和无界数据集</div>
</center>

## 窗口计算
DataFlow提供了3种窗口计算类型，支持窗口把一个数据集切分为有限的数据片，以便于聚合处理。
- 滚动窗口(Tumbling Window)
- 滑动窗口(Sliding Window)
- 会话窗口(Session Window)
- 全局窗口(Global Window)


## 时间域与水位线机制
时间域分为两种类型，即事件时间(event time)和处理时间(processing time)，其中事件时间指事件发生时的系统时间；处理时间指数据处理管道在处理数据时，一个事件被数据处理系统观察到的时间，即数据处理系统的时间。
<center>
    <img src="./img/EventTime-ProcessTime.png">
    <div>事件时间与处理时间关系</div>
</center>
事件时间和处理时间的主要区别在于，事件时间是永远不变的，而事件的处理时间会随着事件在数据管道中被处理而变化。在数据处理过程中，因为系统本身受到一些现实影响（通信延迟、调度算法、处理时长、管道中间数据序列化等），所以会导致这两个时间概念存在差值且动态波动。 <b>借助全局数据处理进度的标记或水位线(Watermark)，可以有效处理迟到乱序的事件，得到正确的数据处理结果</b>

## 分布式异步快照算法

# 参考资料
- Flink设计与实现：核心原理与源码解析