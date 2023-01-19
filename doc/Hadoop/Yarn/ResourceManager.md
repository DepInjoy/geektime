```java
  /**
   * RMActiveServices handles all the Active services in the RM.
   */
  @Private
  public class RMActiveServices extends CompositeService {
      
      

  /**
   * Helper method to create and init {@link #activeServices}. This creates an
   * instance of {@link RMActiveServices} and initializes it.
   *
   * @param fromActive Indicates if the call is from the active state transition
   *                   or the RM initialization.
   */
  protected void createAndInitActiveServices(boolean fromActive) {
    activeServices = new RMActiveServices(this);
    activeServices.fromActive = fromActive;
    activeServices.init(conf);
  }
```



```java
  protected Dispatcher createDispatcher() {
    AsyncDispatcher dispatcher = new AsyncDispatcher("RM Event dispatcher");
    GenericEventTypeMetrics genericEventTypeMetrics =
        GenericEventTypeMetricsManager.
        create(dispatcher.getName(), NodesListManagerEventType.class);
    // We can add more
    dispatcher.addMetrics(genericEventTypeMetrics,
        genericEventTypeMetrics.getEnumClass());
    return dispatcher;
  }

  protected ResourceScheduler createScheduler() {
    String schedulerClassName = conf.get(YarnConfiguration.RM_SCHEDULER,
        YarnConfiguration.DEFAULT_RM_SCHEDULER);
    LOG.info("Using Scheduler: " + schedulerClassName);
    try {
      Class<?> schedulerClazz = Class.forName(schedulerClassName);
      if (ResourceScheduler.class.isAssignableFrom(schedulerClazz)) {
        return (ResourceScheduler) ReflectionUtils.newInstance(schedulerClazz,
            this.conf);
      } else {
        throw new YarnRuntimeException("Class: " + schedulerClassName
            + " not instance of " + ResourceScheduler.class.getCanonicalName());
      }
    } catch (ClassNotFoundException e) {
      throw new YarnRuntimeException("Could not instantiate Scheduler: "
          + schedulerClassName, e);
    }
  }

  /**
   * @brief:    为资源调度器创建事件调度器，事件处理器名称为SchedulerEventDispatcher
   * */
  protected EventHandler<SchedulerEvent> createSchedulerEventDispatcher() {
    String dispatcherName = "SchedulerEventDispatcher";
    EventDispatcher dispatcher;
    // 通过yarn.dispatcher.cpu-monitor.samples-per-min，默认值为60
    // 设置为0,禁用分发线程监控
    int threadMonitorRate = conf.getInt(
        YarnConfiguration.YARN_DISPATCHER_CPU_MONITOR_SAMPLES_PER_MIN,
        YarnConfiguration.DEFAULT_YARN_DISPATCHER_CPU_MONITOR_SAMPLES_PER_MIN);

    if (threadMonitorRate > 0) {
      dispatcher = new SchedulerEventDispatcher(dispatcherName,
          threadMonitorRate);
      ClusterMetrics.getMetrics().setRmEventProcMonitorEnable(true);
    } else {
      dispatcher = new EventDispatcher(this.scheduler, dispatcherName);
    }
    return dispatcher;
  }
```

