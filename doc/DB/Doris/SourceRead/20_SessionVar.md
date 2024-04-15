Doris支持一些会话级参数，参见源码`SessionVariable`

| 参数                                  | 参数意义               | 默认值 |
| :------------------------------------ | ---------------------- | ------ |
| `enable_nereids_planner`              | 开启新查询优化器       | `true` |
| `enable_fallback_to_original_planner` | 开启自动回退到旧优化器 | `true` |

# 并行度



| 参数                                  | 参数意义                                                     | 默认值               |
| :------------------------------------ | ------------------------------------------------------------ | -------------------- |
| `enable_pipeline_engine`              | 使能Pipeline执行引擎                                         | `true`               |
| `parallel_pipeline_task_num`          | 查询并发的Pipeline Task的数目<br/>参数值设置参见`getParallelExecInstanceNum`接口实现 | 0，即CPU核心数的一半 |
| `max_instance_num`                    | 自动并发数上限(`parallel_pipeline_task_num=0`时影响并发度)   | 64                   |
| `enable_pipeline_x_engine`            | 使能PipelineX执行引擎                                        | `false`              |
| `parallel_fragment_exec_instance_num` | 针对扫描节点，设置其在每个 BE 节点上，执行实例的个数，默认为 1。<br/><br/>一个查询计划通常会产生一组 scan range，即需要扫描的数据范围。这些数据分布在多个 BE 节点上。一个 BE节点会有一个或多个scan range。默认情况下，每个 BE 节点的一组scan range只由一个执行实例处理。当机器资源比较充裕时，可以将增加该变量，让更多执行实例同时处理一组scan range，从而提升查询效率。<br/>scan实例的数量决定了上层其他执行节点，如聚合节点，join节点的数量。因此相当于增加了整个查询计划执行的并发度。修改该参数会对大查询效率提升有帮助，但较大数值会消耗更多的机器资源，如CPU、内存、磁盘IO。 | 8                    |

```java
// source code in SessionVariable.java
public int getParallelExecInstanceNum() {
    ConnectContext connectContext = ConnectContext.get();
    if (connectContext != null && connectContext.getEnv() != null && connectContext.getEnv().getAuth() != null) {
        int userParallelExecInstanceNum = connectContext.getEnv().getAuth()
                .getParallelFragmentExecInstanceNum(connectContext.getQualifiedUser());
        if (userParallelExecInstanceNum > 0) {
            return userParallelExecInstanceNum;
        }
    }

    if (getEnablePipelineEngine() && parallelPipelineTaskNum == 0) {
        // 多个backend CPU核心数的最小值
        int size = Env.getCurrentSystemInfo().getMinPipelineExecutorSize();
        int autoInstance = (size + 1) / 2;
        return Math.min(autoInstance, maxInstanceNum);
    } else if (getEnablePipelineEngine()) {
        return parallelPipelineTaskNum;
    } else {
        // 火山模型
        return parallelExecInstanceNum;
    }
}
```



```java
// 计划OlapScanNode类(Full scan of an Olap table)
public int getNumInstances() {
    // In pipeline exec engine, the instance num equals be_num * parallel instance.
    // so here we need count distinct be_num to do the work. make sure get right instance
    if (ConnectContext.get().getSessionVariable().getEnablePipelineEngine()
            && !ConnectContext.get().getSessionVariable().getEnablePipelineXEngine()
            && ConnectContext.get().getSessionVariable().getEnableSharedScan()) {
        return ConnectContext.get().getSessionVariable().getParallelExecInstanceNum();
    }

    // PipelineX执行引擎且忽略存储层数据分布
    if (ConnectContext.get().getSessionVariable().getEnablePipelineXEngine()
            && ConnectContext.get().getSessionVariable().isIgnoreStorageDataDistribution()) {
        return ConnectContext.get().getSessionVariable().getParallelExecInstanceNum();
    }

    return scanRangeLocations.size();
}
```

