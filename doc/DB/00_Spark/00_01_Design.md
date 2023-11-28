Apache Spark 和 Apache Beam设计的基本模型：

<center>
    <img src="https://static001.geekbang.org/resource/image/53/2e/53aa1aad08b11e6c2db5cf8bb584572e.png?wh=4909*3085" width=75% height=75%>
</center>

用有向图进行数据处理描述，数据处理描述语言部分完全可以和后面的运算引擎分离了。有向图可以作为数据处理描述语言和运算引擎的前后端分离协议。

# 为什么需要Spark

既然已经有了看似成熟的Hadoop和MapReduce，那么为什么还需要Spark？可以帮助我们解决什么实际问题？相比于MapReduce，它的优势是什么?



## MapReduce的缺陷

MapReduce被硅谷一线公司淘汰的两个主要原因：维护成本高、时间性能无法达到用户期待。此外：

- MapReduce 模型的抽象层次低，大量的底层逻辑都需要开发者手工完成。
- 只提供Map和Reduce两个操作
- 在 Hadoop 中，每一个 Job 的计算结果都会存储在 HDFS 文件存储系统中，所以每一步计算都要进行硬盘的读取和写入，大大增加了系统的延迟。由于这一原因，MapReduce对迭代算法的处理性能很差，而且很耗资源。因为迭代的每一步都要对 HDFS 进行读写，所以每一步都需要差不多的等待时间。
- 只支持批数据处理，欠缺对流数据处理的支持。



## Spark的优势

Spark进行了更高级别的数据抽象RDD，一次数据读取之后便可在内存中进行多步迭代计算。其最基本的数据抽象叫作弹性分布式数据集(Resilient Distributed Dataset, RDD)，它代表一个可以被分区(partition)的只读数据集，它内部可以有很多分区，每个分区又有大量的数据记录(record)。RDD是 Spark 最基本的数据结构。

- Spark 定义了很多对 RDD 的操作(如 Map、Filter、flatMap、groupByKey 和 Union 等等)。对 RDD 的任何操作都可以像函数式编程中操作内存中的集合一样直观、简便，使得实现数据处理的代码非常简短高效。

- Spark 会把中间数据缓存在内存中，从而减少了硬盘读写而导致的延迟，大大加快了处理速度。Spark 可以把迭代过程中每一步的计算结果都缓存在内存中，所以非常适用于各类迭代算法，下图可以看出：Hadoop 做每一次迭代运算的时间基本相同，而 Spark 除了第一次载入数据到内存以外，别的迭代时间基本可以忽略。

<center>
    <img src="https://static001.geekbang.org/resource/image/54/3d/54e4df946206a4a2168a25af8814843d.png?wh=5083*3500" width=75% height=75%>
    <div>Spark和Hadoop运行逻辑回归算法的运行时间对比</div>
</center>



- 在任务(task)级别上，Spark的并行机制是多线程模型，而MapReduce是多进程模型，其Map和Reduce的Task都是进程。多进程模型便于细粒度控制每个任务占用的资源，但会消耗较多的启动时间。而 Spark 同一节点上的任务以多线程的方式运行在一个 JVM 进程中，可以带来更快的启动速度、更高的 CPU 利用率，以及更好的内存共享。



## Spark是什么

从狭义上来看，Spark 只是 MapReduce 的替代方案，大部分应用场景中，它还要依赖于 HDFS 和 HBase 来存储数据，依赖于 YARN 来管理集群和资源。当然，Spark 并不是一定要依附于 Hadoop 才能生存，它还可以运行在 Apache Mesos、Kubernetes、standalone 等其他云平台上。

<center>
    <img src="https://static001.geekbang.org/resource/image/bc/0c/bc01239280bb853ca1d00c0fb3a8150c.jpg?wh=3604*2329" width=75% height=75%>
</center>

作为通用的数据处理平台，Spark 有五个主要的扩展库，分别是支持结构化数据的 Spark SQL、处理实时数据的 Spark Streaming、用于机器学习的 MLlib、用于图计算的 GraphX、用于统计分析的 SparkR。

有哪些 MapReduce 的缺点是在 Spark 框架中依然存在的？用什么思路可以解决？

> 1. spark本质上还是批处理，只是通过微批处理来实现近实时处理。如果需要实时处理，可以使用apache flink； 
> 2. 小文件处理依然有性能问题； 
> 3. 仍需要手动调优，比如如何让数据合理分区，来避免或者减轻数据倾斜
> 4. 数据分片分不好，导致数据过于集中在某机器导致整体处理速度慢或者无法处理问题。spark还是全靠使用者的分片函数还是自己有方法可以动态调度？ (用Spark的RDD API没有自动优化数据分区，很依赖开发者手动调优)
> 5. 对于实际复杂业务中的多job前后依赖，与业务紧密耦合的异常捕捉，处理机制是否有更好的解决方法？（Spark SQL的执行引擎会有这方面的优化，性能要显著优于RDD。）



# 参考资料

1. [极客时间-大规模数据处理实战](https://time.geekbang.com/column/intro/100025301?tab=catalog)

