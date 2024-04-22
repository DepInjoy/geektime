```scala
/**
* Check whether the resources from the WorkerOffer are enough to run at least one task.
* Returns None if the resources don't meet the task requirements, otherwise returns
* the task resource assignments to give to the next task. Note that the assignments maybe
* be empty if no custom resources are used.
*/
private def resourcesMeetTaskRequirements(
    taskSet: TaskSetManager, availCpus: Int,
    availWorkerResources: Map[String, Buffer[String]]
    ): Option[Map[String, ResourceInformation]] = {
val rpId = taskSet.taskSet.resourceProfileId
val taskSetProf = sc.resourceProfileManager.resourceProfileFromId(rpId)
val taskCpus = ResourceProfile.getTaskCpusOrDefaultForProfile(taskSetProf, conf)
// check if the ResourceProfile has cpus first since that is common case
if (availCpus < taskCpus) return None

// 该Executor可以被分配任务
val tsResources = taskSetProf.getCustomTaskResources()
if (tsResources.isEmpty) return Some(Map.empty)
val localTaskReqAssign = HashMap[String, ResourceInformation]()
// we go through all resources here so that we can make sure they match and also get what the
// assignments are for the next task
for ((rName, taskReqs) <- tsResources) {
    val taskAmount = taskSetProf.getSchedulerTaskResourceAmount(rName)
    availWorkerResources.get(rName) match {
    case Some(workerRes) =>
        if (workerRes.size >= taskAmount) {
        localTaskReqAssign.put(rName, new ResourceInformation(rName,
            workerRes.take(taskAmount).toArray))
        } else {
        return None
        }
    case None => return None
    }
}
Some(localTaskReqAssign.toMap)
}
```

`TaskSetManager::resourceOffer`实现为Executor分配Task
```scala
  def resourceOffer(
      execId: String,
      host: String,
      maxLocality: TaskLocality.TaskLocality,
      taskCpus: Int = sched.CPUS_PER_TASK,
      taskResourceAssignments: Map[String, ResourceInformation] = Map.empty)
    : (Option[TaskDescription], Boolean, Int) = {
    val offerExcluded = taskSetExcludelistHelperOpt.exists { excludeList =>
      excludeList.isNodeExcludedForTaskSet(host) ||
        excludeList.isExecutorExcludedForTaskSet(execId)
    }
    if (!isZombie && !offerExcluded) {
      val curTime = clock.getTimeMillis()
      var allowedLocality = maxLocality

      if (maxLocality != TaskLocality.NO_PREF) {
        allowedLocality = getAllowedLocalityLevel(curTime)
        if (allowedLocality > maxLocality) {
          // We're not allowed to search for farther-away tasks
          allowedLocality = maxLocality
        }
      }

      var dequeuedTaskIndex: Option[Int] = None
      val taskDescription =
        dequeueTask(execId, host, allowedLocality)
          .map { case (index, taskLocality, speculative) =>
            dequeuedTaskIndex = Some(index)
            if (legacyLocalityWaitReset && maxLocality != TaskLocality.NO_PREF) {
              resetDelayScheduleTimer(Some(taskLocality))
            }
            if (isBarrier) {
              barrierPendingLaunchTasks(index) =
                BarrierPendingLaunchTask(execId, host, index,
                  taskLocality, taskResourceAssignments)
              // return null since the TaskDescription for the barrier task is not ready yet
              null
            } else {
              prepareLaunchingTask(execId,host, index, taskLocality, speculative,
                                   taskCpus,taskResourceAssignments, curTime)
            }
          }
      val hasPendingTasks = pendingTasks.all.nonEmpty || pendingSpeculatableTasks.all.nonEmpty
      val hasScheduleDelayReject = taskDescription.isEmpty &&
          maxLocality == TaskLocality.ANY && hasPendingTasks
      (taskDescription, hasScheduleDelayReject, dequeuedTaskIndex.getOrElse(-1))
    } else {
      (None, false, -1)
    }
  }
```