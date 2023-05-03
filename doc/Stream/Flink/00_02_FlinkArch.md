# Flink集群
Flink在运行时包含两种进程：
- JobManager（又称为 JobMaster）：它具有与协调Flink应用程序分布式执行相关的职责：协调Task的分布式执行，包括调度Task、协调Checkpoint以及当Job failover时协调各个Task从 Checkpoint恢复等。该进程由三个组件组成：
    - ResourceManger：负责 Flink 集群中的资源提供、回收、分配-它管理task slots，Flink 为不同的环境和资源提供者（例如 YARN、Kubernetes 和 standalone 部署）实现了对应的ResourceManager.
    - Dispatcher: 提供了一个REST接口来提交Flink应用程序执行，并为每个提交的作业启动一个新的JobMaster; 还运行Flink WebUI用来提供作业执行信息。
    - JobMaster：负责管理单个JobGraph的执行，Flink集群中可以同时运行多个作业，每个作业都有自己的JobMaster。

- TaskManager（又称为 Worker）：执行Dataflow中的Tasks，包括内存Buffer的分配、Data Stream的传递等。TaskManager中的最小资源分配单位时Task Slot，TaskManager中Task Slot的数量表示并发处理的Task数量。需要注意的是，一个Task Slot中可以执行多个Operator。


<center>
    <img src="./img/Flink-Arch.svg">
    <div>事件时间与处理时间关系</div>
</center>
Task是Flink中资源调度的最小单位。

# Task和算子链
<center>
    <img src="./img/tasks_chains.svg">
</center>

# Task Slot和资源

