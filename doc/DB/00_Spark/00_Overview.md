大多数现有的集群计算系统都是基于非循环的数据流模型。即从稳定的物理存储(如分布式文件系统)中加载记录，记录被传入由一组确定性操作构成的DAG(Directed Acyclic Graph，有向无环图)，然后写回稳定存储。DAG数据流图能够在运行时自动实现任务调度和故障恢复。非循环数据流是一种很强大的抽象方法，但仍然有些应用无法使用这种方式描述。这类应用包括：机器学习和图应用中常用的迭代算法(每一步对数据执行相似的函数)；交互式数据挖掘工具(用户反复查询一个数据子集)。基于数据流的框架并不明确支持工作集，所以需要将数据输出到磁盘，然后在每次查询时重新加载，这会带来较大的开销。

针对上述问题，Spark实现了一种分布式的内存抽象，称为弹性分布式数据集
(ResilientDistributed Dataset，RDD)。它支持基于工作集(即多个并行操作重用中间结果的应用)的应用，同时具有数据流模型的特点：自动容错、位置感知性调度和可伸缩性。RDD允许用户在执行多个查询时显式地将工作集缓存在内存中，后续的查询能够重用工作集，这极大地提升了查询速度。

# 架构

<center>
    <img src="./img/00_spark_arch.png">
    <div>Spark整体架构图</div>
</center>
Driver是用户编写的数据处理逻辑，这个逻辑中包含用户创建的SparkContext。SparkContext是用户逻辑与Spark集群主要的交互接口，它会和Cluster Manager交互，包括向它申请计算资源等。Cluster Manager负责集群的资源管理和调度，现在支持Standalone、Apache Mesos和YARN。Worker是Spark集群中可以执行计算任务的节点，负责启动Executor，Worker节点可以有一个或多个Executor。Executor是在Worker节点上为某应用启动的进程，负责运行任务，并将数据存在内存或者磁盘上。Task是Spark作业执行的最小单位。每个Executor都有多个Task，一个Task是一个线程，对应了一个任务。在基于RDD计算时，Task的数量其实就是RDD的分区数，RDD的分区数目决定了总的Task数量。每个Task执行的结果就是生成了目标RDD的一个partition。

<br/><br/>
用户程序从最开始的提交到最终的计算执行，需要经历以下几个阶段：

1. 用户程序创建SparkContext时，新创建的SparkContext实例会连接到Cluster Manager。Cluster Manager会根据用户提交时设置的CPU和内存等信息为本次提交分配计算资源，启动Executor。
2. Driver会将用户程序划分为不同的执行阶段，每个执行阶段由一组完全相同的Task组成，这些Task分别作用于待处理数据的不同分区。在阶段划分完成和Task创建后，Driver会向Executor发送Task。
3. Executor在接收到Task后，会下载Task的运行时依赖，在准备好Task的执行环境后，会开始执行Task，并且将Task的运行状态汇报给Driver。
4. Driver会根据收到的Task的运行状态来处理不同的状态更新。Task分为两种：一种是Shuffle Map Task，它实现数据的重新洗牌，洗牌的结果保存到Executor所在节点的文件系统中；另外一种是Result Task，它负责生成结果数据。
5. Driver会不断地调用Task，将Task发送到Executor执行，在所有的Task都正确执行或者超过执行次数的限制仍然没有执行成功时停止。

# RDD

Spark的目标是为基于工作集的应用(即多个并行操作重用中间结果的应用)提供抽象，同时保持MapReduce及其相关模型的优势特性，即自动容错、位置感知性调度和可伸缩性。RDD比数据流模型更易于编程，同时基于工作集的计算也具有良好的描述能力。

在这些特性中，最难实现的是容错性。一般来说，分布式数据集的容错性有两种方式：数据检查点和记录数据的更新。我们面向的是大规模数据分析，数据检查点操作成本很高：需要通过数据中心的网络连接在机器之间复制庞大的数据集，而网络带宽往往比内存带宽低得多，同时还需要消耗更多的存储资源(在内存中复制数据可以减少需要缓存的数据量，而存储到磁盘则会降低应用程序速度)。所以，我们选择记录更新的方式。但是，如果更新太多，记录更新成本也不低。因此，RDD只支持粗粒度转换，即在大量记录上执行的单个操作。将创建RDD的一系列转换记录下来(即Lineage)，以便恢复丢失的分区。

---
什么是RDD？<b>RDD是只读的、分区记录的集合。</b>RDD只能基于在稳定物理存储中的数据集和其他已有的RDD上执行确定性操作来创建，这些确定性操作称为转换，如map、filter、groupBy、join。RDD不需要物化。RDD含有如何从其他RDD衍生(即计算)出本RDD的相关信息(即Lineage)，因此在RDD部分分区数据丢失的时候可以从物理存储的数据计算出相应的RDD分区。

RDD支持基于工作集的应用，同时具有数据流模型的特点：自动容错、位置感知性调度和可伸缩性。RDD允许用户在执行多个查询时显式地将工作集缓存在内存中，后续的查询能够重用工作集，这极大地提升了查询速度，这是Spark速度非常快的原因之一。当持久化一个RDD后，每一个节点都将把计算的分片结果保存在内存中，并在对此数据集(或者衍生出的数据集)进行的其他动作(action)中重用, 这使得后续的动作变得更加迅速(通常快10倍)。

缓存有可能丢失，或者存储于内存的数据由于内存不足而被删除，需要重新计算。RDD的缓存的容错机制保证了即使缓存丢失也能保证计算的正确执行。通过基于RDD的一系列的转换，丢失的数据会被重算。RDD的各个Partition是相对独立的，因此只需要计算丢失的部分即可，并不需要重算全部Partition。

如果计算特别复杂或者计算耗时特别多，那么缓存丢失对于整个Job的影响是不容忽视的。为了避免缓存丢失重新计算带来的开销，Spark又引入了检查点(checkpoint)机制。缓存是在计算结束后，直接将计算结果通过用户定义的存储级别(存储级别定义了缓存存储的介质，现在支持内存、本地文件系统和Tachyon)写入不同的介质。而检查点不同，它是在计算完成后，重新建立一个Job来计算。为了避免重复计算，推荐先将RDD缓存，这样就能保证检查点的操作可以快速完成。

---

每个RDD有5个主要的属性：
1. 一组分片(Partition)，即数据集的基本组成单位。对于RDD来说，每个分片都会被一个计算任务处理，并决定并行计算的粒度。用户可以在创建RDD时指定RDD的分片个数，如果没有指定，那么就会采用默认值。默认值就是程序所分配到的CPU Core的数目。图3-1描述了分区存储的计算模型，每个分配的存储是由BlockManager实现的。每个分区都会被逻辑映射成BlockManager的一个Block，而这个Block会被一个Task负责计算。

2. 一个计算每个分区的函数。Spark中RDD的计算是以分片为单位的，每个RDD都会实现compute函数以达到这个目的。compute函数会对迭代器进行复合，不需要保存每次计算的结果。

3. RDD之间的依赖关系。RDD的每次转换都会生成一个新的RDD，所以RDD之间就会形成类似于流水线一样的前后依赖关系。在部分分区数据丢失时，Spark可以通过这个依赖关系重新计算丢失的分区数据，而不是对RDD的所有分区进行重新计算。

4. 一个Partitioner，即RDD的分片函数。当前Spark中实现了两种类型的分片函数，一个是基于哈希的HashPartitioner，另外一个是基于范围的RangePartitioner。只有对于key-value的RDD，才会有Partitioner，非key-value的RDD的Parititioner的值是None。Partitioner函数不但决定了RDD本身的分片数量，也决定了parent RDD Shuffle输出时的分片数量。

5. 一个列表，存储存取每个Partition的优先位置(preferred location)。对于一个HDFS文件来说，这个列表保存的就是每个Partition所在的块的位置。按照“移动数据不如移动计算”的理念，Spark在进行任务调度的时候，会尽可能地将计算任务分配到其所要处理数据块的存储位置。

## RDD创建和操作

可以通过两种方式创建RDD：
1. 由一个已经存在的Scala集合创建。
2. 由外部存储系统的数据集创建，包括本地的文件系统，还有所有Hadoop支持的数据集，比如HDFS、Cassandra、HBase、Amazon S3等。

RDD创建后，就可以在RDD上进行数据处理。RDD支持两种操作：
- 转换(transformation)，即从现有的数据集创建一个新的数据集。RDD中的所有转换都是惰性的，也就是说，它们并不会直接计算结果，它们只是记住这些应用到基础数据集(例如一个文件)上的转换动作。只有当发生一个要求返回结果给Driver的动作时，这些转换才会真正运行，这个设计让Spark更加有效率地运行。

- 动作(action)，即在数据集上进行计算后，返回一个值给Driver程序。

## RDD转换和生成DAG
Spark会根据用户提交的计算逻辑中的RDD的转换和动作来生成RDD之间的依赖关系，同时这个计算链也就生成了逻辑上的DAG。

RDD之间的关系可以从两个维度来理解：
- RDD是从哪些RDD转换而来，也就是RDD的parent RDD(s)是什么
- 依赖于parent RDD(s)的哪些Partition(s)。这个关系就是RDD之间的依赖，org.apache.spark.Dependency。根据依赖于parent RDD(s)的Partitions的不同情况，Spark将这种依赖分为两种，一种是宽依赖，一种是窄依赖

### RDD依赖关系
RDD和它依赖的parent RDD(s)的关系有两种不同的类型，即窄依赖(narrow dependency)和宽依赖(wide dependency)。
1. 窄依赖指的是每一个parent RDD的Partition最多被子RDD的一个Partition使用。
2. 宽依赖指的是多个子RDD的Partition会依赖同一个parent RDD的Partition，

宽依赖支持两种Shuffle Manager，即org.apache.spark.shuffle.hash.HashShuffleManager(基于Hash的Shuffle机制)和org.apache.spark.shuffle.sort.SortShuffleManager(基于排序的Shuffle机制).

# 参考资料
1. 《Spark技术内幕：深入解析Spark内核架构设计与实现原理》