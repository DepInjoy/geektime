任务调度模块主要包含两大部分，即DAGScheduler和TaskScheduler，它们负责将用户提交的计算任务按照DAG划分为不同的Stage并且将不同Stage的计算任务提交到集群进行最终的计算。

- DAGScheduler主要负责分析用户提交的应用，并根据计算任务的依赖关系建立DAG，然后将DAG划分为不同的Stage(阶段)，其中每个Stage由可以并发执行的一组Task构成，这些Task的执行逻辑完全相同，只是作用于不同的数据，DAG在不同的资源管理框架(即部署方式，包括Standalone、Mesos、YARN、Local、EC2等)下的实现相同。
- 在DAGScheduler将这组Task划分完成后，会将这组Task提交到TaskScheduler。TaskScheduler通过Cluster Manager在集群中的某个Worker的Executor上启动任务。

# DAGScheduler实现
## DAGScheduler创建
DAGScheduler在SparkContext创建的时候创建的，由于DAGScheduler引用了TaskScheduler，因此需要先创建TaskScheduler，
```scala
  // 创建TaskScheduler
  val (sched, ts) = SparkContext.createTaskScheduler(this, master)
  _schedulerBackend = sched
  _taskScheduler = ts
  // 创建DAGScheduler
  _dagScheduler = new DAGScheduler(this)
```
在DAGSchedule入参`SparkContext`的构造函数如下:
```scala
def this(sc: SparkContext) = this(sc, sc.taskScheduler)
```

`this(sc, sc.taskScheduler)`的实现
```scala
  def this(sc: SparkContext, taskScheduler: TaskScheduler) = {
    this(sc, taskScheduler, sc.listenerBus,
      sc.env.mapOutputTracker.asInstanceOf[MapOutputTrackerMaster],
      sc.env.blockManager.master, sc.env)
  }
```
之后调用`DAGScheduler`的构造函数
```scala
private[spark] class DAGScheduler(
    private[scheduler] val sc: SparkContext,
    private[scheduler] val taskScheduler: TaskScheduler,
    listenerBus: LiveListenerBus,
    mapOutputTracker: MapOutputTrackerMaster,
    blockManagerMaster: BlockManagerMaster,
    env: SparkEnv,
    clock: Clock = new SystemClock()) extends Logging { }
```

```plantuml
@startuml
class SparkContext {
  - var _taskScheduler: TaskScheduler 
  - var _dagScheduler : DAGScheduler
}

class DAGScheduler {
  - val eventProcessLoop : DAGSchedulerEventProcessLoop
}

class DAGSchedulerEventProcessLoop {
  + def onReceive(event: DAGSchedulerEvent): Unit
}

abstract class EventLoop {
  + def post(event: E): Unit
}

class TaskSchedulerImpl {
  var backend: SchedulerBackend
}

DAGScheduler -up-o SparkContext
TaskScheduler -up-o SparkContext

TaskScheduler -o DAGScheduler : 提交Task
SchedulerBackend -o TaskScheduler

DAGSchedulerEventProcessLoop -up-- DAGScheduler
DAGSchedulerEventProcessLoop -down-|> EventLoop : E=DAGSchedulerEvent

MapOutputTrackerMaster -left-o DAGScheduler
MapOutputTrackerMaster -down-|> MapOutputTracker

TaskSchedulerImpl -up-|> TaskScheduler
@enduml
```
1. `org.apache.spark.scheduler.TaskScheduler`是一个trait，作用是为创建它的SparkContext调度任务，即从DAGScheduler接收不同Stage的任务，并向集群提交任务，并为执行特别慢的任务启动备份任务。TaskScheduler是以后实现多种任务调度器的基础，目前`org.apache.spark.scheduler.TaskSchedulerImpl`是它的实现。
2. `org.apache.spark.scheduler.SchedulerBackend`是一个trait，作用是分配当前可用的资源，具体就是向当前等待分配计算资源的Task分配计算资源(即Executor)，并且在分配的Executor上启动Task，完成计算的调度过程。
3. `MapOutputTrackerMaster`是运行在Driver端管理Shuffle Map Task的输出的，下游的Task可以通过`MapOutputTrackerMaster`来获取Shuffle输出的位置信息。
4. `DAGScheduler`将调度抽象为一系列的Event(时间)，例如Job提交对应于`JobSubmitted`，MapStageSubmitted对应于Map Stage提交等(更多定义参看`core/src/main/scala/org/apache/spark/scheduler\DAGSchedulerEvent.scala`实现)，`DAGSchedulerEventProcessLoop`实现事件调度逻辑,例如
```scala
private[scheduler] sealed trait DAGSchedulerEvent

private[scheduler] case class JobSubmitted(...)
  extends DAGSchedulerEvent

private[scheduler] case class MapStageSubmitted(...)
  extends DAGSchedulerEvent
    ....
```




## Job提交
Job提交的调用流程
```scala
org.apache.spark.SparkContext#runJob
  org.apache.spark.scheduler.DAGScheduler#runJob
    org.apache.spark.scheduler.DAGScheduler#submitJob (生成并返回JobWaiter Object)
      eventProcessLoop#post(JobSubmitted(...)) // 提交JobSubmitted Event

org.apache.spark.scheduler.DAGSchedulerEventProcessLoop#onReceive(JobSubmitted)
  org.apache.spark.scheduler.DAGScheduler#handleJobSubmitted
```

```plantuml
class DAGScheduler {
  - val eventProcessLoop : DAGSchedulerEventProcessLoop
}

class DAGSchedulerEventProcessLoop {
  + def onReceive(event: DAGSchedulerEvent): Unit
}

abstract class EventLoop {
  - val eventQueue: BlockingQueue[E]
  - val eventThread = new Thread(name)
  + def post(event: E): Unit
  + def start(): Unit
  + def stop(): Unit
  # def onReceive(event: E): Unit
  # def onError(e: Throwable): Unit
  # def onStop(): Unit
  # def onStart(): Unit
}

DAGSchedulerEventProcessLoop -up-- DAGScheduler : 提交(post)JobSubmitted事件
DAGSchedulerEventProcessLoop -right-|> EventLoop : E=DAGSchedulerEvent
```

Job提交会为这个Job生成一个JobID，并生成一个JobWaiter实例例来监听Job执行状态，JobWaiter会监听Job的执行状态，而Job是由多个Task组成的，只有Job的所有Task都成功完成，Job才标记为成功；任意一个Task失败都会标记该Job失败。

```scala
def runJob[T, U](rdd: RDD[T], func: (TaskContext, Iterator[T]) => U,
    partitions: Seq[Int], callSite: CallSite,
    resultHandler: (Int, U) => Unit, properties: Properties): Unit = {

  // submitJob内部会为Job生成一个JobID，创建并返回一个JobWaiter实例
  val waiter = submitJob(rdd, func, partitions, callSite, resultHandler, properties)
        ......
}

def submitJob[T, U](rdd: RDD[T], func: (TaskContext, Iterator[T]) => U,
    partitions: Seq[Int], callSite: CallSite,
    resultHandler: (Int, U) => Unit, properties: Properties): JobWaiter[U] = {

  // 1. 创建Job ID
  val jobId = nextJobId.getAndIncrement()
  
  // 2. 创建JobListener(JobWaiter)实例来监听Job执行状态
  if (partitions.isEmpty) {
    // 2.1 创建共有0个task的JobWaiter，
            ......
    listenerBus.post(SparkListenerJobStart(jobId, time, Seq.empty, clonedProperties))
    listenerBus.post(SparkListenerJobEnd(jobId, time, JobSucceeded))
    return new JobWaiter[U](this, jobId, 0, resultHandler)
  }

  // 2.2 创建JobWaiter并提交JobSubmitted事件
  val func2 = func.asInstanceOf[(TaskContext, Iterator[_]) => _]
  val waiter = new JobWaiter[U](this, jobId, partitions.size, resultHandler)
  eventProcessLoop.post(JobSubmitted(....))
  waiter
}
```

对于近似估计的Job，DAGScheduler会调用`runApproximateJob`，其逻辑类似，JobWaiter换成了`org.apache.spark.partial.ApproximateActionListener`
```scala
def runApproximateJob[T, U, R](rdd: RDD[T],
    func: (TaskContext, Iterator[T]) => U,
    evaluator: ApproximateEvaluator[U, R],
    callSite: CallSite, timeout: Long,
    properties: Properties): PartialResult[R] = {
    // 1. 创建Job ID
  val jobId = nextJobId.getAndIncrement()

  // 2. 创建JobListener(ApproximateActionListener)实例来监听Job执行状态
  if (rdd.partitions.isEmpty) {
    // 2.1 直接返回
            ......
    listenerBus.post(SparkListenerJobStart(jobId, time, Seq[StageInfo](), clonedProperties))
    listenerBus.post(SparkListenerJobEnd(jobId, time, JobSucceeded))
    return new PartialResult(evaluator.currentResult(), true)
  }


  // 2.2 创建ApproximateActionListener并提交JobSubmitted事件
  val listener = new ApproximateActionListener(rdd, func, evaluator, timeout)
  val func2 = func.asInstanceOf[(TaskContext, Iterator[_]) => _]
  eventProcessLoop.post(JobSubmitted(
    jobId, rdd, func2, rdd.partitions.indices.toArray, callSite, listener,
    JobArtifactSet.getActiveOrDefault(sc), clonedProperties))
  
  // 3. 异步等待结果，timeout毫秒超时
  listener.awaitResult()
}
```

Job通过`DAGSchedulerEventProcessLoop::post(JobSubmitted))`提交JobSubmitted事件，`DAGSchedulerEventProcessLoop::doOnReceive`对接收到的JobSubmitted Event处理，调用`dagScheduler.handleJobSubmitted`,忽略一些异常处理来了解其主执行流程
```scala
private[scheduler] class DAGSchedulerEventProcessLoop(dagScheduler: DAGScheduler)
  extends EventLoop[DAGSchedulerEvent]("dag-scheduler-event-loop") with Logging {

  // The main event loop of the DAG scheduler.
  override def onReceive(event: DAGSchedulerEvent): Unit = {
    doOnReceive(event)
  }

  private def doOnReceive(event: DAGSchedulerEvent): Unit = event match {
    // 通过createResultStage创建ResultStage
    case JobSubmitted(jobId, rdd, func, partitions, callSite, listener, artifacts, properties) =>
      dagScheduler.handleJobSubmitted(jobId, rdd, func, partitions, callSite, listener, artifacts,
        properties)

    // 通过getOrCreateShuffleMapStage创建ShuffleMapStage
    case MapStageSubmitted(jobId, dependency, callSite, listener, artifacts, properties) =>
      dagScheduler.handleMapStageSubmitted(jobId, dependency, callSite, listener, artifacts,
        properties)
      				......
  }
```
可见，JobSubmitted的事件，会触发调用`dagScheduler.handleJobSubmitted`，接下来了解一下它的实现。
```scala
private[scheduler] def handleJobSubmitted(jobId: Int,
    finalRDD: RDD[_], func: (TaskContext, Iterator[_]) => _,
    partitions: Array[Int], callSite: CallSite, listener: JobListener,
    artifacts: JobArtifactSet, properties: Properties): Unit = {
  var finalStage: ResultStage = null
  // 1. 创建finalStage,这个接口会获取宽依赖创建ShuffleMapStage
  finalStage = createResultStage(finalRDD, func, partitions, jobId, callSite)
  
  // Job submitted, clear internal data.
  barrierJobIdToNumTasksCheckFailures.remove(jobId)

  // 2. 创建ActiveJob
  val job = new ActiveJob(jobId, finalStage, callSite, listener, artifacts, properties)
  clearCacheLocs()

  val jobSubmissionTime = clock.getTimeMillis()
  jobIdToActiveJob(jobId) = job
  activeJobs += job

  // 3. 为finalStage设置ActiveJob
  finalStage.setActiveJob(job)
  val stageIds = jobIdToStageIds(jobId).toArray
  val stageInfos = stageIds.flatMap(id => stageIdToStage.get(id).map(_.latestInfo))
  listenerBus.post(SparkListenerJobStart(job.jobId, jobSubmissionTime, stageInfos,
      Utils.cloneProperties(properties)))

  // 4. Stage提交,这里会向TaskScheduler提交Task
  submitStage(finalStage)
}
```

## Job监听
JobWaiter会监听Job的执行状态

```plantuml
class ApproximateActionListener {
  + def awaitResult(): PartialResult[R]
}
note top : 近似估计Job监听

class JobWaiter {}

interface JobListener {
  + def taskSucceeded(index: Int, result: Any): Unit
  + def jobFailed(exception: Exception): Unit
}
JobWaiter -down-|> JobListener
ApproximateActionListener -down-|> JobListener

```

```scala
org.apache.spark.scheduler.DAGScheduler#handleJobSubmitted
  getShuffleDependenciesAndResourceProfiles // 获取到 shuffleDeps
  // 如果存在shuffle，创建ShuffleMapStage
    
  org.apache.spark.scheduler.DAGScheduler#getOrCreateParentStages
    org.apache.spark.scheduler.DAGScheduler#getOrCreateShuffleMapStage
      org.apache.spark.scheduler.DAGScheduler#createShuffleMapStage
  org.apache.spark.scheduler.DAGScheduler#getOrCreateParentStages()
  org.apache.spark.scheduler.DAGScheduler#createResultStage
```






## Stage划分
Job提交后，DAGSchedulerEventProcessLoop对接收到的`JobSubmitted`事件处理，调用`DAGScheduler::handleJobSubmitted`将DAG划分为不同的Stage(阶段)，其中每个Stage可以由一组并发的Task组成，这些Task的执行逻辑相同，只是作用于完全不同的数据。

Spark用`class Stage`进行Stage的进行抽象表达，根据RDD和它依赖的父RDD(s)的关系分为两种类型，即窄依赖(narrow dependency)用`ResultStage`结构表达和宽依赖(wide dependency)用`ShuffleMapStage`数据结构表达。

```plantuml
@startuml
abstract class Stage {
  - val jobIds = new HashSet[Int]
  + def findMissingPartitions():Seq[Int]
}
class ShuffleMapStage {

}
note top: 宽依赖

class ResultStage {
  - var _activeJob: Option[ActiveJob]
  + def findMissingPartitions():Seq[Int]
}
note top: 窄依赖

ShuffleMapStage -down-|> Stage
ResultStage -down-|> Stage
@enduml
```

```
org.apache.spark.scheduler.DAGScheduler#handleJobSubmitted
  org.apache.spark.scheduler.DAGScheduler#createResultStage(创建finalStage)
```

```scala
/** Submits stage, but first recursively submits any missing parents. */
private def submitStage(stage: Stage): Unit = {
  val jobId = activeJobForStage(stage)
  if (jobId.isDefined) {
    /** 
      DAGScheduler维护了三个HashSet
        1. waitingStages(父Stage未完成)
        2. runningStages(正在执行中)
        3. failedStages(失败后重新提交的Stage) 
    */
    if (!waitingStages(stage) && !runningStages(stage) && !failedStages(stage)) {
      if (stage.getNextAttemptId >= maxStageAttempts) {
        // Job重新提交超过尝试上限，停止该Stage
        val reason = s"$stage (name=${stage.name}) has been resubmitted for the maximum " +
            s"allowable number of times: ${maxStageAttempts}, which is the max value of " +
            s"config `spark.stage.maxAttempts` and `spark.stage.maxConsecutiveAttempts`."
        abortStage(stage, reason, None)
      } else {
        val missing = getMissingParentStages(stage).sortBy(_.id)
        if (missing.isEmpty) {
          // 所有parent stage都已完成，提交该stage所包含的task
          submitMissingTasks(stage, jobId.get)
        } else {
          for (parent <- missing) {
            // 存在parent stage未提交，递归地提交
            submitStage(parent)
          }
          waitingStages += stage
        }
      }
    }
  } else {
    // 无效的Stage，直接停止
    abortStage(stage, "No active job for stage " + stage.id, None)
  }
}
```

## Task生成
org.apache.spark.scheduler.DAGScheduler#handleJobSubmitted，生成finalStage后就会为该Job生成一个org.apache.spark.scheduler.ActiveJob，并准备计算这个finalStage
```scala
private[scheduler] def handleJobSubmitted(...): Unit = {
  var finalStage: ResultStage = null
  finalStage = createResultStage(finalRDD, func, partitions, jobId, callSite)

          ......
  val job = new ActiveJob(jobId, finalStage, callSite, listener, artifacts, properties)
          ......
  
  finalStage.setActiveJob(job)
  
  // 调用submitMissingTasks提交Stage包含的Task
  submitStage(finalStage)
        ......
}
```
```plantuml
@startuml
class TaskSet {}
abstract class Task {}

interface Schedulable {
  - var parent: Pool
}

class TaskSchedulerImpl {
  - val taskSetsByStageIdAndAttempt = new HashMap\n\t[Int, HashMap[Int, TaskSetManager]]
  - var schedulableBuilder: SchedulableBuilder
}
Task -down-o TaskSet

ShuffleMapTask -down-|> Task
ResultTask -down-|> Task

TaskSet -down-o TaskSetManager
TaskSetManager .|> Schedulable

TaskSetManager -left-o TaskSchedulerImpl
@enduml
```

### 创建TaskScheduler和SchedulerBackend
```plantuml
abstract class YarnSchedulerBackend {}

SchedulerBackend -up-o SparkContext : 创建scheduler backend
TaskScheduler -up-o SparkContext : 创建task scheduler

TaskSchedulerImpl -up-|> TaskScheduler : 实现

SchedulerBackend -right-|> ExecutorBackend
LocalSchedulerBackend -up-|> SchedulerBackend

StandaloneSchedulerBackend -up-|>  CoarseGrainedSchedulerBackend
CoarseGrainedSchedulerBackend -up-|> SchedulerBackend : 混入


YarnSchedulerBackend -up-|> SchedulerBackend
YarnClusterSchedulerBackend -up-|> YarnSchedulerBackend
```
```scala
org.apache.spark.SparkContext#createTaskScheduler
```
### Task提交

```
-- 在Driver端执行
org.apache.spark.scheduler.TaskSchedulerImpl#submitTasks
  org.apache.spark.scheduler.TaskSchedulerImpl#createTaskSetManager
  org.apache.spark.scheduler.SchedulableBuilder#addTaskSetManager
  org.apache.spark.scheduler.CoarseGrainedSchedulerBackend#reviveOffers
    makeOffers
      buildWorkerOffer
      org.apache.spark.scheduler.TaskSchedulerImpl#resourceOffers

-- 在Executor上执行
org.apache.spark.executor.CoarseGrainedExecutorBackend.receiveWithLogging#launchTask
org.apache.spark.executor.Executor#launchTask     
```
`TaskSchedulerImpl::submitTasks`
```scala


```

`TaskSchedulerImpl::resourceOffers`响应CoarseGrainedSchedulerBackend的资源调度请求，为每个Task具体分配资源。


- `spark.task.cpus`配置参数，默认值为1，用于设置每个Task所需要的cores(线程数)。

- `spark.scheduler.mode`设置的调度策略。默认为FIFO，

  ```scala
  org.apache.spark.scheduler.Pool
  
  private[spark] class Pool(val poolName: String,
  	val schedulingMode: SchedulingMode,
  	initMinShare: Int, initWeight: Int)
    extends Schedulable with Logging {
    
    private val taskSetSchedulingAlgorithm: SchedulingAlgorithm = {
      schedulingMode match {
        case SchedulingMode.FAIR =>
          new FairSchedulingAlgorithm()
        case SchedulingMode.FIFO =>
          new FIFOSchedulingAlgorithm()
      }
    }
  			......
  }
  ```

`TaskSchedulerImpl::resourceOffers`响应资源调度请求，为每个Task分配资源，该函数的输入是Executor列表，输出是`TaskDescription`二维数组，`TaskDescription`包含了Task ID，Executor ID和Task执行环境的依赖信息等。

```scala
  def resourceOffers(offers: IndexedSeq[WorkerOffer],
      isAllFreeResources: Boolean = true): Seq[Seq[TaskDescription]] = synchronized {
    var newExecAvail = false
    for (o <- offers) {
      if (!hostToExecutors.contains(o.host)) {
        hostToExecutors(o.host) = new HashSet[String]()
      }
      if (!executorIdToRunningTaskIds.contains(o.executorId)) {
        // 有新的executor加入
        hostToExecutors(o.host) += o.executorId
        executorAdded(o.executorId, o.host)
        executorIdToHost(o.executorId) = o.host
        executorIdToRunningTaskIds(o.executorId) = HashSet[Long]()
        newExecAvail = true
      }
    }
    val hosts = offers.map(_.host).distinct
    for ((host, Some(rack)) <- hosts.zip(getRacksForHosts(hosts))) {
      hostsByRack.getOrElseUpdate(rack, new HashSet[String]()) += host
    }

    // 1. 如果开启组织在失败Executor上重新调度任务
    //    过滤掉排除在外的执行器节点,避免单独创建线程和同步开销
    healthTrackerOpt.foreach(_.applyExcludeOnFailureTimeout())
    val filteredOffers = healthTrackerOpt.map { healthTracker =>
      offers.filter { offer =>
        !healthTracker.isNodeExcluded(offer.host) &&
          !healthTracker.isExecutorExcluded(offer.executorId)
      }
    }.getOrElse(offers)

    // 2. 随机打散,避免将Task集中分配给某些机器
    val shuffledOffers = shuffleOffers(filteredOffers)

    // 3. 构建分配给每个worker的tasks
    // 	  offer代表Executor列表,Spark cores概念表示并行执行的线程数
    //    根据spark.task.cpus配置参数(每个Task所需要的cores(线程数)
    //    因此, 需要的Task数量 = cores / CPUS_PER_TASK
    val tasks = shuffledOffers.map(o => new ArrayBuffer[TaskDescription](o.cores / CPUS_PER_TASK))
    val availableResources = shuffledOffers.map(_.resources).toArray
    val availableCpus = shuffledOffers.map(o => o.cores).toArray
    val resourceProfileIds = shuffledOffers.map(o => o.resourceProfileId).toArray
    // 根据用户spark.scheduler.mode设置的调度策略
    //   1. FIFO: FIFOSchedulingAlgorithm::comparator规则
    //   2. Fair: FairSchedulingAlgorithm::comparator规则
    // 对TaskSetManager进行排序
    val sortedTaskSets = rootPool.getSortedTaskSetQueue
    for (taskSet <- sortedTaskSets) {
      if (newExecAvail) {
        // 重新计算该TaskSetManager的就近原则
        taskSet.executorAdded()
      }
    }

    // 为根据调度策略排序好的的TaskSetManager列表分配资源
    // 按照就近原则进行分配,其中优先分配顺序:
    // PROCESS_LOCAL, NODE_LOCAL, NO_PREF, RACK_LOCAL, ANY
    for (taskSet <- sortedTaskSets) { /// -- todo
      // we only need to calculate available slots if using barrier scheduling, otherwise the
      // value is -1
      val numBarrierSlotsAvailable = if (taskSet.isBarrier) {
        val rpId = taskSet.taskSet.resourceProfileId
        val availableResourcesAmount = availableResources.map { resourceMap =>
          // available addresses already takes into account if there are fractional
          // task resource requests
          resourceMap.map { case (name, addresses) => (name, addresses.length) }
        }
        calculateAvailableSlots(this, conf, rpId, resourceProfileIds, availableCpus,
          availableResourcesAmount)
      } else {
        -1
      }
      // Skip the barrier taskSet if the available slots are less than the number of pending tasks.
      if (taskSet.isBarrier && numBarrierSlotsAvailable < taskSet.numTasks) {
        // Skip the launch process.
        // TODO SPARK-24819 If the job requires more slots than available (both busy and free
        // slots), fail the job on submit.
        logInfo(s"Skip current round of resource offers for barrier stage ${taskSet.stageId} " +
          s"because the barrier taskSet requires ${taskSet.numTasks} slots, while the total " +
          s"number of available slots is $numBarrierSlotsAvailable.")
      } else {
        var launchedAnyTask = false
        var noDelaySchedulingRejects = true
        var globalMinLocality: Option[TaskLocality] = None
        for (currentMaxLocality <- taskSet.myLocalityLevels) {
          var launchedTaskAtCurrentMaxLocality = false
          do {
            val (noDelayScheduleReject, minLocality) = resourceOfferSingleTaskSet(
              taskSet, currentMaxLocality, shuffledOffers, availableCpus,
              availableResources, tasks)
            launchedTaskAtCurrentMaxLocality = minLocality.isDefined
            launchedAnyTask |= launchedTaskAtCurrentMaxLocality
            noDelaySchedulingRejects &= noDelayScheduleReject
            globalMinLocality = minTaskLocality(globalMinLocality, minLocality)
          } while (launchedTaskAtCurrentMaxLocality)
        }

        if (!legacyLocalityWaitReset) {
          if (noDelaySchedulingRejects) {
            if (launchedAnyTask &&
              (isAllFreeResources || noRejectsSinceLastReset.getOrElse(taskSet.taskSet, true))) {
              taskSet.resetDelayScheduleTimer(globalMinLocality)
              noRejectsSinceLastReset.update(taskSet.taskSet, true)
            }
          } else {
            noRejectsSinceLastReset.update(taskSet.taskSet, false)
          }
        }

        if (!launchedAnyTask) {
          taskSet.getCompletelyExcludedTaskIfAny(hostToExecutors).foreach { taskIndex =>
              // If the taskSet is unschedulable we try to find an existing idle excluded
              // executor and kill the idle executor and kick off an abortTimer which if it doesn't
              // schedule a task within the timeout will abort the taskSet if we were unable to
              // schedule any task from the taskSet.
              // Note 1: We keep track of schedulability on a per taskSet basis rather than on a per
              // task basis.
              // Note 2: The taskSet can still be aborted when there are more than one idle
              // excluded executors and dynamic allocation is on. This can happen when a killed
              // idle executor isn't replaced in time by ExecutorAllocationManager as it relies on
              // pending tasks and doesn't kill executors on idle timeouts, resulting in the abort
              // timer to expire and abort the taskSet.
              //
              // If there are no idle executors and dynamic allocation is enabled, then we would
              // notify ExecutorAllocationManager to allocate more executors to schedule the
              // unschedulable tasks else we will abort immediately.
              executorIdToRunningTaskIds.find(x => !isExecutorBusy(x._1)) match {
                case Some ((executorId, _)) =>
                  if (!unschedulableTaskSetToExpiryTime.contains(taskSet)) {
                    healthTrackerOpt.foreach(blt => blt.killExcludedIdleExecutor(executorId))
                    updateUnschedulableTaskSetTimeoutAndStartAbortTimer(taskSet, taskIndex)
                  }
                case None =>
                  //  Notify ExecutorAllocationManager about the unschedulable task set,
                  // in order to provision more executors to make them schedulable
                  if (Utils.isDynamicAllocationEnabled(conf)) {
                    if (!unschedulableTaskSetToExpiryTime.contains(taskSet)) {
                      logInfo("Notifying ExecutorAllocationManager to allocate more executors to" +
                        " schedule the unschedulable task before aborting" +
                        s" stage ${taskSet.stageId}.")
                      dagScheduler.unschedulableTaskSetAdded(taskSet.taskSet.stageId,
                        taskSet.taskSet.stageAttemptId)
                      updateUnschedulableTaskSetTimeoutAndStartAbortTimer(taskSet, taskIndex)
                    }
                  } else {
                    // Abort Immediately
                    logInfo("Cannot schedule any task because all executors excluded from " +
                      "failures. No idle executors can be found to kill. Aborting stage " +
                      s"${taskSet.stageId}.")
                    taskSet.abortSinceCompletelyExcludedOnFailure(taskIndex)
                  }
              }
          }
        } else {
          // We want to defer killing any taskSets as long as we have a non excluded executor
          // which can be used to schedule a task from any active taskSets. This ensures that the
          // job can make progress.
          // Note: It is theoretically possible that a taskSet never gets scheduled on a
          // non-excluded executor and the abort timer doesn't kick in because of a constant
          // submission of new TaskSets. See the PR for more details.
          if (unschedulableTaskSetToExpiryTime.nonEmpty) {
            logInfo("Clearing the expiry times for all unschedulable taskSets as a task was " +
              "recently scheduled.")
            // Notify ExecutorAllocationManager as well as other subscribers that a task now
            // recently becomes schedulable
            dagScheduler.unschedulableTaskSetRemoved(taskSet.taskSet.stageId,
              taskSet.taskSet.stageAttemptId)
            unschedulableTaskSetToExpiryTime.clear()
          }
        }

        if (launchedAnyTask && taskSet.isBarrier) {
          val barrierPendingLaunchTasks = taskSet.barrierPendingLaunchTasks.values.toArray
          // Check whether the barrier tasks are partially launched.
          if (barrierPendingLaunchTasks.length != taskSet.numTasks) {
            if (legacyLocalityWaitReset) {
              // Legacy delay scheduling always reset the timer when there's a task that is able
              // to be scheduled. Thus, whenever there's a timer reset could happen during a single
              // round resourceOffer, tasks that don't get or have the preferred locations would
              // always reject the offered resources. As a result, the barrier taskset can't get
              // launched. And if we retry the resourceOffer, we'd go through the same path again
              // and get into the endless loop in the end.
              val errorMsg = s"Fail resource offers for barrier stage ${taskSet.stageId} " +
                s"because only ${barrierPendingLaunchTasks.length} out of a total number " +
                s"of ${taskSet.numTasks} tasks got resource offers. We highly recommend " +
                "you to use the non-legacy delay scheduling by setting " +
                s"${LEGACY_LOCALITY_WAIT_RESET.key} to false to get rid of this error."
              logWarning(errorMsg)
              taskSet.abort(errorMsg)
              throw SparkCoreErrors.sparkError(errorMsg)
            } else {
              val curTime = clock.getTimeMillis()
              if (curTime - taskSet.lastResourceOfferFailLogTime >
                TaskSetManager.BARRIER_LOGGING_INTERVAL) {
                logInfo("Releasing the assigned resource offers since only partial tasks can " +
                  "be launched. Waiting for later round resource offers.")
                taskSet.lastResourceOfferFailLogTime = curTime
              }
              barrierPendingLaunchTasks.foreach { task =>
                // revert all assigned resources
                availableCpus(task.assignedOfferIndex) += task.assignedCores
                task.assignedResources.foreach { case (rName, rInfo) =>
                  availableResources(task.assignedOfferIndex)(rName).appendAll(rInfo.addresses)
                }
                // re-add the task to the schedule pending list
                taskSet.addPendingTask(task.index)
              }
            }
          } else {
            // All tasks are able to launch in this barrier task set. Let's do
            // some preparation work before launching them.
            val launchTime = clock.getTimeMillis()
            val addressesWithDescs = barrierPendingLaunchTasks.map { task =>
              val taskDesc = taskSet.prepareLaunchingTask(
                task.execId,
                task.host,
                task.index,
                task.taskLocality,
                false,
                task.assignedCores,
                task.assignedResources,
                launchTime)
              addRunningTask(taskDesc.taskId, taskDesc.executorId, taskSet)
              tasks(task.assignedOfferIndex) += taskDesc
              shuffledOffers(task.assignedOfferIndex).address.get -> taskDesc
            }

            // materialize the barrier coordinator.
            maybeInitBarrierCoordinator()

            // Update the taskInfos into all the barrier task properties.
            val addressesStr = addressesWithDescs
              // Addresses ordered by partitionId
              .sortBy(_._2.partitionId)
              .map(_._1)
              .mkString(",")
            addressesWithDescs.foreach(_._2.properties.setProperty("addresses", addressesStr))

            logInfo(s"Successfully scheduled all the ${addressesWithDescs.size} tasks for " +
              s"barrier stage ${taskSet.stageId}.")
          }
          taskSet.barrierPendingLaunchTasks.clear()
        }
      }
    }

    // TODO SPARK-24823 Cancel a job that contains barrier stage(s) if the barrier tasks don't get
    // launched within a configured time.
    if (tasks.nonEmpty) {
      hasLaunchedTask = true
    }
    return tasks.map(_.toSeq)
  }
```



```scala
  private def resourceOfferSingleTaskSet(
      taskSet: TaskSetManager,
      maxLocality: TaskLocality,
      shuffledOffers: Seq[WorkerOffer],
      availableCpus: Array[Int],
      availableResources: Array[Map[String, Buffer[String]]],
      tasks: IndexedSeq[ArrayBuffer[TaskDescription]])
    : (Boolean, Option[TaskLocality]) = {
    var noDelayScheduleRejects = true
    var minLaunchedLocality: Option[TaskLocality] = None
    // nodes and executors that are excluded for the entire application have already been
    // filtered out by this point
    for (i <- shuffledOffers.indices) {
      val execId = shuffledOffers(i).executorId
      val host = shuffledOffers(i).host
      val taskSetRpID = taskSet.taskSet.resourceProfileId

      // check whether the task can be scheduled to the executor base on resource profile.
      if (sc.resourceProfileManager
        .canBeScheduled(taskSetRpID, shuffledOffers(i).resourceProfileId)) {
        val taskResAssignmentsOpt = resourcesMeetTaskRequirements(taskSet, availableCpus(i),
          availableResources(i))
        taskResAssignmentsOpt.foreach { taskResAssignments =>
          try {
            val prof = sc.resourceProfileManager.resourceProfileFromId(taskSetRpID)
            val taskCpus = ResourceProfile.getTaskCpusOrDefaultForProfile(prof, conf)
            val (taskDescOption, didReject, index) =
              taskSet.resourceOffer(execId, host, maxLocality, taskCpus, taskResAssignments)
            noDelayScheduleRejects &= !didReject
            for (task <- taskDescOption) {
              val (locality, resources) = if (task != null) {
                tasks(i) += task
                addRunningTask(task.taskId, execId, taskSet)
                (taskSet.taskInfos(task.taskId).taskLocality, task.resources)
              } else {
                assert(taskSet.isBarrier, "TaskDescription can only be null for barrier task")
                val barrierTask = taskSet.barrierPendingLaunchTasks(index)
                barrierTask.assignedOfferIndex = i
                barrierTask.assignedCores = taskCpus
                (barrierTask.taskLocality, barrierTask.assignedResources)
              }

              minLaunchedLocality = minTaskLocality(minLaunchedLocality, Some(locality))
              availableCpus(i) -= taskCpus
              assert(availableCpus(i) >= 0)
              resources.foreach { case (rName, rInfo) =>
                // Remove the first n elements from availableResources addresses, these removed
                // addresses are the same as that we allocated in taskResourceAssignments since it's
                // synchronized. We don't remove the exact addresses allocated because the current
                // approach produces the identical result with less time complexity.
                availableResources(i)(rName).remove(0, rInfo.addresses.size)
              }
            }
          } catch {
            case e: TaskNotSerializableException =>
              logError(s"Resource offer failed, task set ${taskSet.name} was not serializable")
              // Do not offer resources for this task, but don't throw an error to allow other
              // task sets to be submitted.
              return (noDelayScheduleRejects, minLaunchedLocality)
          }
        }
      }
    }
    (noDelayScheduleRejects, minLaunchedLocality)
  }
```

```plantuml
Interface SchedulableBuilder {
  + def rootPool: Pool
  + def buildPools(): Unit
  + def addTaskSetManager(manager: Schedulable, properties: Properties): Unit
}

FIFOSchedulableBuilder -down-|> SchedulableBuilder
FairSchedulableBuilder -down-|> SchedulableBuilder
```


```plantuml
class TaskSchedulerImpl {
  var backend: SchedulerBackend
}

abstract class SchedulerBackend {
  + def start(): Unit
  + def stop(): Unit
  + def stop(exitCode: Int): Unit
  + def reviveOffers(): Unit
}

TaskSchedulerImpl -right-|> TaskScheduler
SchedulerBackend -up-o TaskSchedulerImpl
```





```plantuml
@startuml
class SparkContext {
 - var _dagScheduler: DAGScheduler
 - var _taskScheduler: TaskScheduler
 - var _schedulerBackend: SchedulerBackend
}

class SparkEnv {
  - val mapOutputTracker: MapOutputTracker
  - val conf: SparkConf
}

class DAGScheduler {
 - MapOutputTrackerMaster
 - val taskScheduler: TaskScheduler
 - val sc: SparkContext
}

class MapOutputTracker {
  + getStatistics(dep: ShuffleDependency[_, _, _]): \n\tMapOutputStatistics
}

MapOutputTrackerMaster -down-* DAGScheduler

DAGScheduler -right-- SparkContext
SparkEnv -up-* SparkContext
TaskScheduler -down-* SparkContext
SchedulerBackend -down-* SparkContext

MapOutputTrackerMaster -down.|> MapOutputTracker
@enduml
```
#  DAGScheduler

DAGScheduler主要负责分析用户提交的应用，并根据计算任务的依赖关系建立DAG，然后将DAG划分为不同的Stage(阶段)，其中每个Stage由可以并发执行的一组Task构成，这些Task的执行逻辑完全相同，只是作用于不同的数据，DAG在不同的资源管理框架下实现相同。


```plantuml
@startuml
abstract class JobListener {
  + taskSucceeded(index: Int, result: Any): Unit
  + jobFailed(exception: Exception): Unit
}

JobWaiter -down-|> JobListener
@enduml
```

JobListener是一种特质，
```scala
// org.apache.spark.scheduler

private[spark] trait JobListener {
  def taskSucceeded(index: Int, result: Any): Unit
  def jobFailed(exception: Exception): Unit
}
```
```scala
private[spark] class JobWaiter[T](
    dagScheduler: DAGScheduler,
    val jobId: Int,
    totalTasks: Int,
    resultHandler: (Int, T) => Unit)
  extends JobListener with Logging {

  }
```





## 创建Stage
```scala
private[scheduler] class DAGSchedulerEventProcessLoop(dagScheduler: DAGScheduler)
  extends EventLoop[DAGSchedulerEvent]("dag-scheduler-event-loop") with Logging {

  // The main event loop of the DAG scheduler.
  override def onReceive(event: DAGSchedulerEvent): Unit = {
    doOnReceive(event)
  }

  private def doOnReceive(event: DAGSchedulerEvent): Unit = event match {
    // 通过createResultStage创建ResultStage
    case JobSubmitted(jobId, rdd, func, partitions, callSite, listener, artifacts, properties) =>
      dagScheduler.handleJobSubmitted(jobId, rdd, func, partitions, callSite, listener, artifacts,
        properties)

    // 通过getOrCreateShuffleMapStage创建ShuffleMapStage
    case MapStageSubmitted(jobId, dependency, callSite, listener, artifacts, properties) =>
      dagScheduler.handleMapStageSubmitted(jobId, dependency, callSite, listener, artifacts,
        properties)
      				......
  }
```



```scala
  private def getOrCreateParentStages(shuffleDeps: HashSet[ShuffleDependency[_, _, _]],
      firstJobId: Int): List[Stage] = {
    shuffleDeps.map { shuffleDep =>
      getOrCreateShuffleMapStage(shuffleDep, firstJobId)
    }.toList
  }
```

```scala
  private[scheduler] def getShuffleDependenciesAndResourceProfiles(
      rdd: RDD[_]): (HashSet[ShuffleDependency[_, _, _]], HashSet[ResourceProfile]) = {
    val parents = new HashSet[ShuffleDependency[_, _, _]]
    val resourceProfiles = new HashSet[ResourceProfile]
    val visited = new HashSet[RDD[_]]
    val waitingForVisit = new ListBuffer[RDD[_]]
    waitingForVisit += rdd
    while (waitingForVisit.nonEmpty) {
      val toVisit = waitingForVisit.remove(0)
      if (!visited(toVisit)) {
        visited += toVisit
        Option(toVisit.getResourceProfile).foreach(resourceProfiles += _)
        toVisit.dependencies.foreach {
          case shuffleDep: ShuffleDependency[_, _, _] =>
            parents += shuffleDep
          case dependency =>
            waitingForVisit.prepend(dependency.rdd)
        }
      }
    }
    (parents, resourceProfiles)
  }
```

## 提交Stage
```
org.apache.spark.SparkContext#submitMapStage
org.apache.spark.scheduler.DAGScheduler#submitMapStage


```

```scala
class SimpleFutureAction[T] private[spark](jobWaiter: JobWaiter[_], resultFunc: => T)
  extends FutureAction[T] {}
trait FutureAction[T] extends Future[T] 
```

```scala
private[spark] class MapOutputStatistics(val shuffleId: Int, val bytesByPartitionId: Array[Long])
```

```scala
private[spark] class MapOutputTrackerMaster(
    conf: SparkConf,
    private[spark] val broadcastManager: BroadcastManager,
    private[spark] val isLocal: Boolean)
  extends MapOutputTracker(conf) {

  // Return statistics about all of the outputs for a given shuffle.
  def getStatistics(dep: ShuffleDependency[_, _, _]): MapOutputStatistics = {
  }
```