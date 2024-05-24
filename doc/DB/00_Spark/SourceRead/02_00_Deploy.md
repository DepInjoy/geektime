Spark的Cluster Manager有以下几种部署模式：Standalone，Mesos，YARN，EC2，Local。

# 运行模式
`SparkContext`的创建过程中，会通过传入的Master URL的值来确定不同的运行模式，并且创建不同的`SchedulerBackend`和`TaskScheduler`，参见`org.apache.spark.SparkContext#createTaskScheduler`

## Local模式
Master URL如果使用以下方式，那么就是以本地方式启动Spark：
1. `local`使用一个工作线程来运行计算任务，不会重新计算失败的计算任务。
2. `local[N]/local[*]`,对于local[N]，使用N个工作线程；对于`local[*]`，工作线程的数量取决于本机的CPU Core的数目，保证逻辑上一个工作线程可以使用一个CPU Core。和local一样，不会重新计算失败的计算任务。
3. `local[threads，maxFailures]`, `threads`指定工作线程的数目；`maxFailures`设置计算任务最大的失败重试次数。
4. `local-cluster[numSlaves，coresPerSlave，memoryPerSlave]`, 伪分布式模式，本机会运行Master和Worker。其中numSlaves设置了Worker的数目；coresPerSlave设置了Worker所能使用的CPU Core的数目；memoryPerSlave设置了每个Worker所能使用的内存数。

对于前3种方式，内部实现是相同的，区别就是启动的工作线程数和计算失败时重试的次数不一样。对于local模式来说，`SchedulerBackend`的实现是`org.apache.spark.scheduler.local.LocalSchedulerBackend`。`TaskScheduler`的实现是`org.apache.spark.scheduler.TaskSchedulerImpl`。


对于第4种伪分布式模式，实际上是在本地机器模拟了一个分布式环境，除了Master和Worker都运行在本机外，与Standalone模式并无区别。`SchedulerBackend`的实现是`org.apache.spark.scheduler.cluster.StandaloneSchedulerBackend`。`TaskScheduler`的实现也是`org.apache.spark.scheduler.TaskSchedulerImpl`。

## Mesos
Spark可以通过选项`spark.mesos.coarse`来设置是采用粗粒度的调度模式还是细粒度的调度模式。
### 粗粒度调度
`org.apache.spark.scheduler.cluster.CoarseGrainedSchedulerBackend`

### 细粒度调度

## Yarn
### YARN Cluster模式
YARN Cluster模式，就是通过Hadoop YARN来调度Spark Application所需要的资源。

### YARN Client模式
该模式和YARN Cluster模式的区别在于，用户提交的Application的SparkContext是在本机上运行，适合Application本身需要在本地进行交互的场景；而YARNCluster中，所有的计算都是在YARN的节点上运行的。

# 模块整体架构
Deploy模块采用的也是典型的Master/Slave架构，其中Master负责整个集群的资源调度和Application的管理。Slave(即Worker)接收Master的资源分配调度命令后启动Executor，由Executor完成最终的计算任务, Client则负责Application的创建和向Master注册Application，并且负责接收来自Executor的状态更新和计算结果等。

Deploy模块主要包含3个子模块：Master、Worker、Client，它们之间的通信通过AKKA完成。对于Master和Worker，它们本身就是一个Actor，可以直接通过AKKA实现通信。Client虽然本身不是一个Actor，这三者的主要职责如下：
1. Master：接收Worker的注册并管理所有的Worker，接收Client提交的Application，FIFO调度等待的Application并向Worker提交。
2. Worker：向Master注册自己，根据Master发送的Application配置进程环境，并启动`StandaloneExecutorBackend`。
3. Client：向Master注册并监控Application。当用户创建SparkContext时会实例化`SparkDeploySchedulerBackend`，而实例化`SparkDeploySchedulerBackend`的同时会启动Client，通过向Client传递启动参数和Application有关信息，Client向Master发送请求注册Application并且在计算节点上启动`StandaloneExecutorBackend`。

# 消息传递机制
Deploy模块之间主要通过AKKA通信，通过对各个子模块之间消息通信协议的梳理，可以分析每个子模块的功能职责

## Master和Worker
Master作为整个集群的管理者，需要Worker通过注册、汇报状态来维护整个集群的运行状态，并且通过这些状态来决定资源调度策略等。


Worker向Master发送的消息主要包含三类：
1. 注册：Worker启动时需要向Master注册，注册时需要汇报自身的信息。
2. 状态汇报：汇报Executor和Driver的运行状态；在Master故障恢复时，需要汇报Worker上当前运行的Executor和Driver的信息。
3. 报活心跳：Worker每隔指定周期会向Master发送报活的心跳。

Master向Worker发送的消息除了响应Worker的注册外，还有一些控制命令，包括让Worker重新注册、让Worker启动Executor或者Driver、停止Executor和Driver等。

## Master和Client

## Client和Executor

# 集群启动

## Master启动

## Worker启动

# 集群容错

# Master HA实现
Standalone是一个采用Master/Slave的典型架构，Master会出现单点故障(Single Point of Failure，SPOF)问题, Spark可以选用ZooKeeper来实现高可用性(High Availability，HA)


# 参考资料
1. Spark技术内幕：深入解析Spark内核架构设计与实现原理