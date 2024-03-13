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
| `parallel_fragment_exec_instance_num` | BE节点并行执行执行实例个数                                   | 8                    |

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

