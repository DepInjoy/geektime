任务调度模块主要包含两大部分，即DAGScheduler和TaskScheduler，它们负责将用户提交的计算任务按照DAG划分为不同的Stage并且将不同Stage的计算任务提交到集群进行最终的计算。

- DAGScheduler主要负责分析用户提交的应用，并根据计算任务的依赖关系建立DAG，然后将DAG划分为不同的Stage(阶段)，其中每个Stage由可以并发执行的一组Task构成，这些Task的执行逻辑完全相同，只是作用于不同的数据，DAG在不同的资源管理框架(即部署方式，包括Standalone、Mesos、YARN、Local、EC2等)下的实现相同。
- 在DAGScheduler将这组Task划分完成后，会将这组Task提交到TaskScheduler。TaskScheduler通过Cluster Manager在集群中的某个Worker的Executor上启动任务。

# DAGScheduler实现
## Job提交

## Job监听
`JobListener`是一种特质，在其上派生出`JobWaiter`会监听Job的执行状态

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



# 任务调度实现





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

## 创建TaskScheduler和SchedulerBackend
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
## Task提交

```scala
// 在Driver端执行
org.apache.spark.scheduler.TaskSchedulerImpl#submitTasks
  org.apache.spark.scheduler.TaskSchedulerImpl#createTaskSetManager
  org.apache.spark.scheduler.SchedulableBuilder#addTaskSetManager
  org.apache.spark.scheduler.CoarseGrainedSchedulerBackend#reviveOffers
  org.apache.spark.scheduler.CoarseGrainedSchedulerBackend.DriverEndpoint#makeOffers
  	org.apache.spark.scheduler.CoarseGrainedSchedulerBackend.DriverEndpoint#buildWorkerOffer
  	org.apache.spark.scheduler.TaskSchedulerImpl#resourceOffers
	org.apache.spark.scheduler.CoarseGrainedSchedulerBackend.DriverEndpoint#launchTasks
	// 这里executorEndpoint.send(LaunchTask(......)

// 在Executor上执行
org.apache.spark.executor.CoarseGrainedExecutorBackend.receiveWithLogging#launchTask
org.apache.spark.executor.Executor#launchTask     
```
`TaskSchedulerImpl::submitTasks`


`TaskSchedulerImpl::resourceOffers`响应CoarseGrainedSchedulerBackend的资源调度请求，为每个Task具体分配资源。


- `spark.task.cpus`配置参数，默认值为1，用于设置每个Task所需要的cores(线程数)。

- `spark.scheduler.mode`设置的调度策略。默认为FIFO，



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

    // 1. 如果开启spark.excludeOnFailure.enabled为true
    //    如果某节点的任务多次失败，Spark会将这个节点排除在集群之外
    //	  这意味着不会用于之后的任务调度
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
    for (taskSet <- sortedTaskSets) {
      // Barrier Task -- todo
      // 如果采用Barrier执行模式，计算可用的Barrier Slot数, 否则为-1
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

    if (tasks.nonEmpty) {
      hasLaunchedTask = true
    }
    return tasks.map(_.toSeq)
  }
```


忽略异常处理了解offer resource的实现
```scala
private def resourceOfferSingleTaskSet(taskSet: TaskSetManager,
    maxLocality: TaskLocality, shuffledOffers: Seq[WorkerOffer],
    availableCpus: Array[Int],
    availableResources: Array[Map[String, Buffer[String]]],
    tasks: IndexedSeq[ArrayBuffer[TaskDescription]])
  : (Boolean, Option[TaskLocality]) = {
  var noDelayScheduleRejects = true
  var minLaunchedLocality: Option[TaskLocality] = None

  // 顺序遍历当前的Executor
  for (i <- shuffledOffers.indices) {
    // 获取当前Executor的Executor ID和host name
    val execId = shuffledOffers(i).executorId
    val host = shuffledOffers(i).host
    val taskSetRpID = taskSet.taskSet.resourceProfileId

    // 根据ResourceProfile检查该Executor是否可以分配Task
    if (sc.resourceProfileManager.canBeScheduled(
          taskSetRpID, shuffledOffers(i).resourceProfileId)) {
      // 检查WorkOffer提供的Resource是否足够运行至少一个Task
      // 如果不满足，返回None,否则返回一组分配task resource
      val taskResAssignmentsOpt = resourcesMeetTaskRequirements(
          	taskSet, availableCpus(i), availableResources(i))
      // taskResAssignmentsOpt不为None,该Executor可以分配任务
      taskResAssignmentsOpt.foreach { taskResAssignments =>
        val prof = sc.resourceProfileManager.resourceProfileFromId(taskSetRpID)
        val taskCpus = ResourceProfile.getTaskCpusOrDefaultForProfile(prof, conf)
        // 调用TaskSetManager为Executor分配Task
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

## Task调度
在org.apache.spark.scheduler.TaskSchedulerImpl#submitTasks关键性的调用是``
```scala
override def submitTasks(taskSet: TaskSet): Unit = {
    val tasks = taskSet.tasks
    this.synchronized {
      // 创建TaskSetManager
      val manager = createTaskSetManager(taskSet, maxTaskFailures)
              ......
      
      schedulableBuilder.addTaskSetManager(manager, manager.taskSet.properties)
              ......
    }
    backend.reviveOffers()
  }
```
Spark支持FIFO和FAIR两种调度策略，通过`spark.scheduler.mode`配置参数设置的调度策略，默认为FIFO。`schedulableBuilder`在创建在`在org.apache.spark.scheduler.TaskSchedulerImpl#initialize`,相关实现如下：
```scala
val rootPool: Pool = new Pool("", schedulingMode, 0, 0)
            ......
def initialize(backend: SchedulerBackend): Unit = {
  this.backend = backend
  schedulableBuilder = {
    // schedulingMode取决于spark.scheduler.mode配置
    schedulingMode match {
      // FIFO对应的FIFOSchedulableBuilder
      case SchedulingMode.FIFO =>
        new FIFOSchedulableBuilder(rootPool)
      // FAIR对应的是FairSchedulableBuilder
      case SchedulingMode.FAIR =>
        new FairSchedulableBuilder(rootPool, sc)
    }
  }

  // 对于FIFO调度, buildPools是空
  // FAIR调度，根据配置文件创建调度树
  // 值得注意的是Pool也是一种Schedulable
  schedulableBuilder.buildPools()
}
```

```plantuml
Interface SchedulableBuilder {
  + def rootPool: Pool
  + def buildPools(): Unit
  + def addTaskSetManager(manager: Schedulable, properties: Properties): Unit
}

class Pool {
  - val schedulableQueue = new ConcurrentLinkedQueue[Schedulable]
  - val taskSetSchedulingAlgorithm: SchedulingAlgorithm
  + def getSortedTaskSetQueue(): ArrayBuffer[TaskSetManager]
}


Interface SchedulingAlgorithm {
  + def comparator(s1: Schedulable, s2: Schedulable): Boolean
}
FIFOSchedulableBuilder -down.|> SchedulableBuilder
FairSchedulableBuilder -down.|> SchedulableBuilder

Pool -|> Schedulable : 继承
Pool -up-* SchedulableBuilder
SchedulingAlgorithm -up-* Pool
FIFOSchedulingAlgorithm -up.|> SchedulingAlgorithm
FairSchedulingAlgorithm -up.|> SchedulingAlgorithm

TaskSetManager -left-o Pool : 包含
TaskSetManager -down-|> Schedulable : 继承
```

`Pool::getSortedTaskSetQueue`用SchedulingAlgorithm对TaskSetManager排序，进而实现控制Task的调度顺序。
```scala
// org.apache.spark.scheduler.Pool

private[spark] class Pool(val poolName: String,
  val schedulingMode: SchedulingMode, initMinShare: Int,
  initWeight: Int) extends Schedulable with Logging {
  
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

### FIFO调度
采用FIFO任务调度，首先要保证Job ID较小的先被调度，如果是同一个Job，那么Stage ID小的先被调度。
```scala
private[spark] class FIFOSchedulingAlgorithm extends SchedulingAlgorithm {
  override def comparator(s1: Schedulable, s2: Schedulable): Boolean = {
    // 实际上就是Job ID
    val priority1 = s1.priority
    val priority2 = s2.priority
    var res = math.signum(priority1 - priority2)
    if (res == 0) {
      // Job ID相同，比较Stage ID
      val stageId1 = s1.stageId
      val stageId2 = s2.stageId
      res = math.signum(stageId1 - stageId2)
    }
    res < 0
  }
}
```

### FAIR调度
对于FAIR来说，rootPool包含了一组Pool，这些Pool构成了一棵调度树，其中这棵树的叶子节点就是TaskSetManager。FAIR调度需要在rootPool的基础上根据配置文件(参见`spark.scheduler.allocation.file`配置参数)来构建这课调度树，默认采用`fairscheduler.xml`，一个合法的配置文件格式如下：
```
<allocations>
  <pool name="production">
    <schedulingMode>FAIR</schedulingMode>
    <weight>1</weight>
    <minShare>2</minShare>
  </pool>
  <pool name="test">
    <schedulingMode>FIFO</schedulingMode>
    <weight>2</weight>
    <minShare>3</minShare>
  </pool>
</allocations>
```
对于一个POOL可以指定一些系列的属性：weight表示重要性的概念，minShare(定义最小保留容量)和schedulingMode(指定给定POOL中作业是以FIFO还是FAIR方式调度)。

忽略异常处理了解调度树的构建
```scala
private[spark] class FairSchedulableBuilder(val rootPool: Pool, sc: SparkContext)
  extends SchedulableBuilder with Logging {

  val schedulerAllocFile = sc.conf.get(SCHEDULER_ALLOCATION_FILE)
  val DEFAULT_SCHEDULER_FILE = "fairscheduler.xml"

  override def buildPools(): Unit = {
    var fileData: Option[(InputStream, String)] = None

    fileData = schedulerAllocFile.map { f =>
      // 根据spark.scheduler.allocation.file配置创建InputStream
      val filePath = new Path(f)
      val fis = filePath.getFileSystem(sc.hadoopConfiguration).open(filePath)
      Some((fis, f))
    }.getOrElse {
      // spark.scheduler.allocation.file没设置
      // 以fairscheduler.xml创建InputStream
      val is = Utils.getSparkClassLoader.getResourceAsStream(DEFAULT_SCHEDULER_FILE)
      if (is != null) {
        Some((is, DEFAULT_SCHEDULER_FILE))
      } else {
        // 不存在fairscheduler.xml文件，根据默认参数创建POOL
        val schedulingMode = SchedulingMode.withName(sc.conf.get(SCHEDULER_MODE))
        rootPool.addSchedulable(new Pool(
          DEFAULT_POOL_NAME, schedulingMode, DEFAULT_MINIMUM_SHARE, DEFAULT_WEIGHT))
        None
      }
    }

    // 以is InputStream来构建FAIR POOL
    fileData.foreach { case (is, fileName) => buildFairSchedulerPool(is, fileName) }
    
    fileData.foreach { case (is, fileName) => is.close() }

    // 创建default pool
    buildDefaultPool()
  }

  private def buildDefaultPool(): Unit = {
    if (rootPool.getSchedulableByName(DEFAULT_POOL_NAME) == null) {
      // 创建Pool，作为一个Schedulable添加到pool中
      val pool = new Pool(DEFAULT_POOL_NAME, DEFAULT_SCHEDULING_MODE,
        DEFAULT_MINIMUM_SHARE, DEFAULT_WEIGHT)
      rootPool.addSchedulable(pool)
    }
  }

  private def buildFairSchedulerPool(is: InputStream, fileName: String): Unit = {
    val xml = XML.load(is)
    for (poolNode <- (xml \\ POOLS_PROPERTY)) {
      val poolName = (poolNode \ POOL_NAME_PROPERTY).text
      val schedulingMode = getSchedulingModeValue(poolNode, poolName,
        DEFAULT_SCHEDULING_MODE, fileName)
      val minShare = getIntValue(poolNode, poolName, MINIMUM_SHARES_PROPERTY,
        DEFAULT_MINIMUM_SHARE, fileName)
      val weight = getIntValue(poolNode, poolName, WEIGHT_PROPERTY,
        DEFAULT_WEIGHT, fileName)

      // POOL中通过schedulingMode指定调度方式
      // 实现内部采用一种调度算法来确定TaskSetManager的调度顺序
      rootPool.addSchedulable(new Pool(poolName, schedulingMode, minShare, weight))
    }
  }
}
```
<center>
  <img src="../img/Fair_Pool_Arch.jpg" width=75% height=75%>
  <div> FAIR调度逻辑图<div>
</center>

对于FAIR，首先是挂到rootPool下面的pool先确定调度顺序，然后在每个pool内部使用相同的算法来确定TaskSetManager的调度顺序,其调度算法实现如下：
```scala
private[spark] class FairSchedulingAlgorithm extends SchedulingAlgorithm {
  override def comparator(s1: Schedulable, s2: Schedulable): Boolean = {
    val minShare1 = s1.minShare
    val minShare2 = s2.minShare
    val runningTasks1 = s1.runningTasks
    val runningTasks2 = s2.runningTasks
    val s1Needy = runningTasks1 < minShare1
    val s2Needy = runningTasks2 < minShare2
    val minShareRatio1 = runningTasks1.toDouble / math.max(minShare1, 1.0)
    val minShareRatio2 = runningTasks2.toDouble / math.max(minShare2, 1.0)
    val taskToWeightRatio1 = runningTasks1.toDouble / s1.weight.toDouble
    val taskToWeightRatio2 = runningTasks2.toDouble / s2.weight.toDouble

    var compare = 0
    if (s1Needy && !s2Needy) {
      // 1. 如果s1满足最小保留容量,s2不满足，s1先调度
      return true
    } else if (!s1Needy && s2Needy) {
      // 2. s1不满足最小保留容量，s2满足，s2先调度
      return false
    } else if (s1Needy && s2Needy) {
      // 3. s1和s2同时满足最小保留容量，需要最小保留量分数小的先调度
      compare = minShareRatio1.compareTo(minShareRatio2)
    } else {
      // 4. s1和s2都不满足最小保留容量，权重大的小调度
      compare = taskToWeightRatio1.compareTo(taskToWeightRatio2)
    }
    if (compare < 0) {
      true
    } else if (compare > 0) {
      false
    } else {
      s1.name < s2.name
    }
  }
}
```


## Task运算结果处理

Task在Executor执行完成时，会通过向Driver发送StatusUpdate的消息来通知Driver任务的状态更新为TaskState.FINISHED。Driver首先会将任务的状态更新通知TaskScheduler，然后会在这个Executor上重新分配新的计算任务。

`org.apache.spark.scheduler.Task-SchedulerImpl#statusUpdate`忽略异常，了解主执行逻辑

```scala
def statusUpdate(tid: Long, state: TaskState, serializedData: ByteBuffer): Unit = {
  var failedExecutor: Option[String] = None
  var reason: Option[ExecutorLossReason] = None
  synchronized {
    try {
      Option(taskIdToTaskSetManager.get(tid)) match {
        case Some(taskSet) =>
          if (state == TaskState.LOST) {
            val execId = taskIdToExecutorId.getOrElse(tid, {
              val errorMsg = "taskIdToTaskSetManager.contains(tid) <=> taskIdToExecutorId.contains(tid)"
              taskSet.abort(errorMsg)
              throw new SparkException(errorMsg)
            })
            if (executorIdToRunningTaskIds.contains(execId)) {
              reason = Some(ExecutorProcessLost(
                  s"Task $tid was lost, so marking the executor as lost as well."))
              removeExecutor(execId, reason.get)
              failedExecutor = Some(execId)
            }
          }
          if (TaskState.isFinished(state)) {
            // Task状态是FINISHED, FAILED, KILLED, LOST
            // 认为Task结束,清理本地数据结构
            cleanupTaskState(tid)
            // TaskSetManager标记Task结束
            taskSet.removeRunningTask(tid)
            if (state == TaskState.FINISHED) {
              // Task成功完成,处理计算结果
              taskResultGetter.enqueueSuccessfulTask(taskSet, tid, serializedData)
            } else if (Set(TaskState.FAILED, TaskState.KILLED, TaskState.LOST).contains(state)) {
              // Task失败,处理任务失败情况
              taskResultGetter.enqueueFailedTask(taskSet, tid, state, serializedData)
            }
          }
          if (state == TaskState.RUNNING) {
            taskSet.taskInfos(tid).launchSucceeded()
          }
        case None =>
          logError(......)
      }
    } catch {
      case e: Exception => logError("Exception in statusUpdate", e)
    }
  }

  // 有Executor失败，通知DAGScheduler
  if (failedExecutor.isDefined) {
    dagScheduler.executorLost(failedExecutor.get, reason.get)
    backend.reviveOffers()
  }
}
```



### 处理任务成功执行的机制

忽略异常处理来理解，Task传回的结果，Driver端对结果的处理，可以了解Spark对成功完成的Task的处理。

首先，从Task传回的结果用TaskResult表示，它有两种情况
```plantuml
Interface TaskResult {}
class IndirectTaskResult {}
note top : 计算结果存储在远程worker的BlockManager中

class DirectTaskResult {}
note top: 直接包含Task处理完成的结果
IndirectTaskResult -down.|> TaskResult
DirectTaskResult -down.|> TaskResult
```
```scala
def enqueueSuccessfulTask(taskSetManager: TaskSetManager,tid: Long,
    serializedData: ByteBuffer): Unit = {
  getTaskResultExecutor.execute(new Runnable {
    override def run(): Unit = Utils.logUncaughtExceptions {
      val (result, size) = serializer.get().deserialize[TaskResult[_]](serializedData) match {
        
        // 1. 处理从Task传回的结果,有两种情况
        // 1.1 接受的结果直接是计算结果
        case directResult: DirectTaskResult[_] =>
          // 确定计算结果是否符合要求
          if (!taskSetManager.canFetchMoreResults(directResult.valueByteBuffer.size)) {
            // 计算结果不符合要求，kill task避免其称为僵尸Task
            scheduler.handleFailedTask(taskSetManager, tid, TaskState.KILLED, TaskKilled(
              "Tasks result size has exceeded maxResultSize"))
            return
          }
          directResult.value(taskResultSerializer.get())
          (directResult, serializedData.limit().toLong)
          
        // 1.2 结果存在远程的worker的BlockManager，需要向远程Work请求获取结果
        case IndirectTaskResult(blockId, size) =>
          if (!taskSetManager.canFetchMoreResults(size)) {
            // 计算结果不符合要求,从远程worker删除结果并kill task
            sparkEnv.blockManager.master.removeBlock(blockId)
            scheduler.handleFailedTask(taskSetManager, tid, TaskState.KILLED, TaskKilled(
              "Tasks result size has exceeded maxResultSize"))
            return
          }
          // 1.2.1 从远程BlockManager获取计算结果
          scheduler.handleTaskGettingResult(taskSetManager, tid)
          val serializedTaskResult = sparkEnv.blockManager.getRemoteBytes(blockId)
          if (serializedTaskResult.isEmpty) {
            // 在Executor的任务执行完成和Driver端取结果之间
            // Executor所在机器可能出现故障或者其他错误，导致获取结果失败
            scheduler.handleFailedTask(taskSetManager, tid, TaskState.FINISHED, TaskResultLost)
            return
          }
          // 1.2.2 对结果反序列化
          val deserializedResult = SerializerHelper
            .deserializeFromChunkedBuffer[DirectTaskResult[_]](
              serializer.get(), serializedTaskResult.get)
          deserializedResult.value(taskResultSerializer.get())
          // 1.2.3 将远程结果删除
          sparkEnv.blockManager.master.removeBlock(blockId)
          (deserializedResult, size)
      }

      // 2. 设置从executor中接受到的task result的大小
      result.accumUpdates = result.accumUpdates.map { a =>
        if (a.name == Some(InternalAccumulator.RESULT_SIZE)) {
          val acc = a.asInstanceOf[LongAccumulator]
          acc.setValue(size)
          acc
        } else {
          a
        }
      }

      // 3. 通知taskScheduler处理获取到的结果
      scheduler.handleSuccessfulTask(taskSetManager, tid, result)
    }
  })
}
```

`scheduler.handleSuccessfulTask(taskSetManager, tid, result)`负责处理获取到的结果，处理过程的调用栈

```
1. org.apache.spark.scheduler.TaskSchedulerImpl#handleSuccessfulTask
2. org.apache.spark.scheduler.TaskSetManager#handleSuccessfulTask
   // 如果TaskSetManager的所有Task都已经成功完成，那么从rootPool中删除它
3. org.apache.spark.scheduler.DAGScheduler#taskEnded
4. org.apache.spark.scheduler.DAGScheduler#DAGSchedulerEventProcessLoop#post(CompletionEvent)
5. org.apache.spark.scheduler.DAGScheduler#handleTaskCompletion
```

核心处理在于`org.apache.spark.scheduler.DAGScheduler#handleTaskCompletion`，它完成对计算结果的处理，这里会调用`org.pache.spark.scheduler.JobWaiter`来告知调用者任务已经结束。这里忽略一些异常出和重试的逻辑了解其实现流程。

```scala
private[scheduler] def handleTaskCompletion(event: CompletionEvent): Unit = {
  val task = event.task
  val stageId = task.stageId

  outputCommitCoordinator.taskCompleted(stageId, task.stageAttemptId,
    task.partitionId, event.taskInfo.attemptNumber, event.reason)

  // 处理Event时, Stage可能已经结束了
  if (!stageIdToStage.contains(task.stageId)) {
    postTaskEnd(event)
    return
  }

  val stage = stageIdToStage(task.stageId)
  event.reason match {
    case Success =>
      task match {
        case rt: ResultTask[_, _] =>
          val resultStage = stage.asInstanceOf[ResultStage]
          resultStage.activeJob match {
            case Some(job) =>
              // Only update the accumulator once for each result task.
              if (!job.finished(rt.outputId)) {
                updateAccumulators(event)
              }
            case None => // Ignore update if task's job has finished.
          }
        case _ =>
          updateAccumulators(event)
      }
    case _: ExceptionFailure | _: TaskKilled => updateAccumulators(event)
    case _ =>
  }
  if (trackingCacheVisibility) {
    // Update rdd blocks' visibility status.
    blockManagerMaster.updateRDDBlockVisibility(
      event.taskInfo.taskId, visible = event.reason == Success)
  }

  postTaskEnd(event)

  event.reason match {
    case Success =>
      // An earlier attempt of a stage (which is zombie) may still have running tasks. If these
      // tasks complete, they still count and we can mark the corresponding partitions as
      // finished if the stage is determinate. Here we notify the task scheduler to skip running
      // tasks for the same partition to save resource.
      if (!stage.isIndeterminate && task.stageAttemptId < stage.latestInfo.attemptNumber()) {
        taskScheduler.notifyPartitionCompletion(stageId, task.partitionId)
      }

      task match {
        case rt: ResultTask[_, _] =>
          val resultStage = stage.asInstanceOf[ResultStage]
          resultStage.activeJob match {
            case Some(job) =>
              if (!job.finished(rt.outputId)) {
                job.finished(rt.outputId) = true
                job.numFinished += 1
                // 所有的Task已经处理完成，将Stage标记为结束
                if (job.numFinished == job.numPartitions) {
                  markStageAsFinished(resultStage)
                  cancelRunningIndependentStages(job, s"Job ${job.jobId} is finished.")
                  cleanupStateForJobAndIndependentStages(job)
                  taskScheduler.killAllTaskAttempts(stageId,shouldInterruptTaskThread(job),
                      reason = "Stage finished")
                  listenerBus.post(SparkListenerJobEnd(job.jobId,
					clock.getTimeMillis(), JobSucceeded))
                }


                // 普通Job的listener是JobWaiter,调用JobWaiter::taskSucceeded
                // 运行用户自定义的结果处理函数处理，这里可能会抛出异常
                try {
                  job.listener.taskSucceeded(rt.outputId, event.result)
                } catch {
                  case e: Throwable if !Utils.isFatalError(e) =>
                    // 标记任务失败
                    job.listener.jobFailed(new SparkDriverExecutionException(e))
                }
              }
            case None =>
              logInfo("Ignoring result from " + rt + " because its job has finished")
          }

        case smt: ShuffleMapTask =>
          val shuffleStage = stage.asInstanceOf[ShuffleMapStage]
          // Ignore task completion for old attempt of indeterminate stage
          val ignoreIndeterminate = stage.isIndeterminate &&
            task.stageAttemptId < stage.latestInfo.attemptNumber()
          if (!ignoreIndeterminate) {
            shuffleStage.pendingPartitions -= task.partitionId
            val status = event.result.asInstanceOf[MapStatus]
            val execId = status.location.executorId
            logDebug("ShuffleMapTask finished on " + execId)
            if (executorFailureEpoch.contains(execId) &&
              smt.epoch <= executorFailureEpoch(execId)) {
              logInfo(s"Ignoring possibly bogus $smt completion from executor $execId")
            } else {
              // The epoch of the task is acceptable (i.e., the task was launched after the most
              // recent failure we're aware of for the executor), so mark the task's output as
              // available.
              mapOutputTracker.registerMapOutput(
                shuffleStage.shuffleDep.shuffleId, smt.partitionId, status)
            }
          } else {
            logInfo(s"Ignoring $smt completion from an older attempt of indeterminate stage")
          }

          if (runningStages.contains(shuffleStage) && shuffleStage.pendingPartitions.isEmpty) {
            if (!shuffleStage.shuffleDep.isShuffleMergeFinalizedMarked &&
              shuffleStage.shuffleDep.getMergerLocs.nonEmpty) {
              checkAndScheduleShuffleMergeFinalize(shuffleStage)
            } else {
              processShuffleMapStageCompletion(shuffleStage)
            }
          }
      }
          ......
  }
}
```

`JobWaiter::taskSucceeded`的处理

```scala
override def taskSucceeded(index: Int, result: Any): Unit = {
  // resultHandler call must be synchronized in case resultHandler itself is not thread safe.
  synchronized {
    resultHandler(index, result.asInstanceOf[T])
  }
  if (finishedTasks.incrementAndGet() == totalTasks) {
    jobPromise.success(())
  }
}
```



#  DAGScheduler






## 创建Stage

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