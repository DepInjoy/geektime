# 背景和目标
PipelineX 执行引擎 是 Doris 在 2.1 版本加入的实验性功能。目标是为了解决Doris pipeline引擎的四大问题：
1. 执行并发上，当前Doris执行并发收到两个因素的制约，一个是fe设置的参数，另一个是受存储层bucket数量的限制，这样的静态并发使得执行引擎无法充分利用机器资源。
2. 执行逻辑上，当前Doris有一些固定的额外开销，例如表达式部分各个instance彼此独立，而instance的初始化参数有很多公共部分，所以需要额外进行很多重复的初始化步骤。
3. 调度逻辑上，当前pipeline的调度器会把阻塞task全部放入一个阻塞队列中，由一个线程负责轮询并从阻塞队列中取出可执行task放入runnable队列，所以在有查询执行的过程中，会固定有一个核的资源作为调度的开销。
4. profile方面，目前pipeline无法为用户提供简单易懂的指标。

预期目标:
1. 执行并发上，<b><font color=33FF5E>依赖local exchange使pipelinex充分并发</font></b>，可以让数据被均匀分布到不同的task中，尽可能减少数据倾斜，此外，pipelineX也将不再受存储层tablet数量的制约。
2. 执行逻辑上，<b><font color=33FF5E>多个pipeline task共享同一个pipeline的全部共享状态</font></b>，例如表达式和一些const变量，<b><font color=33FF5E>消除了额外的初始化开销</font></b>。
3. 调度逻辑上，所有pipeline task的阻塞条件都使用Dependency进行了封装，通过外部事件(例如rpc完成)触发task的执行逻辑进入runnable队列，从而消除了阻塞轮询线程的开销。
4. profile：为用户提供简单易懂的指标。

# 使用接口
[Doris官网关于PipelineX的介绍](https://doris.apache.org/zh-CN/docs/query-acceleration/pipeline-x-execution-engine/)

用户接口变更,其中会话级参数

`enable_pipeline_x_engine` : 设置为true，则 BE在进行查询执行时就会默认将SQL的执行模型转变PipelineX的执行方式。
```sql
set enable_pipeline_x_engine = true;
```
`enable_local_shuffle` : 设置为true则打开local shuffle优化。local shuffle将尽可能将数据均匀分布给不同的pipeline task从而尽可能避免数据倾斜。
```sql
set enable_local_shuffle = true;
```

`ignore_storage_data_distribution` : 设置为true，则表示忽略存储层的数据分布。结合local shuffle一起使用，则pipelineX引擎的并发能力将不再受到存储层tablet数量的制约，从而充分利用机器资源。
```sql
set ignore_storage_data_distribution = true;
```

---
# 设计

存在的问题(动机)
> The PipelineX execution engine is an experimental feature in Doris 2.1.0, expected to address the four major issues of the Doris pipeline engine:
> 1. In terms of execution concurrency, Doris is currently constrained by two factors: one is the parameters set by FE, and the other is limited by the number of buckets. This concurrent strategy prevents the execution engine from fully utilizing machine resources.
> 2. In terms of execution logic, Doris currently has some fixed additional overhead. For example, the common expression for all instances will be initialized multiple times due to independence between all instances.
> 3. In terms of scheduling logic, the scheduler of the current pipeline will put all blocking tasks into a blocking queue, and a blocking queue scheduler will be responsible for polling and extracting executable tasks from the blocking queue and placing them in the runnable queue. Therefore, during the query execution process, a CPU core will always be occupied to do scheduling instead of execution.
> 4. In terms of profile, currently the pipeline cannot provide users concise and clear metrics.

要解决的问题(目标)
> 1. In terms of execution concurrency, pipelineX introduces local exchange optimization to fully utilize CPU resources, and distribute data evenly across different tasks to minimize data skewing. In addition, pipelineX will no longer be constrained by the number of tablets.
> 2. Logically, multiple pipeline tasks share all shared states of the same pipeline and eliminate additional initialization overhead, such as expressions and some const variables.
> 3. In terms of scheduling logic, the blocking conditions of all pipeline tasks are encapsulated using Dependency, and the execution logic of the tasks is triggered by external events (such as rpc completion) to enter the runnable queue, thereby eliminating the overhead of blocking polling threads.
> 4. Profile: Provide users with simple and easy to understand metrics

```plantuml
@startuml
FragmentMgr -> PipelineXFragmentContext:new
FragmentMgr -[#FF9F33]> PipelineXFragmentContext:prepare
activate PipelineXFragmentContext #FF9F33
PipelineXFragmentContext -> RuntimeState:create_unique\n[创建Global Runtime State]
PipelineXFragmentContext -> PipelineXFragmentContext:add_pipeline\n[root_pipeline]
note right of PipelineXFragmentContext #DAF7A6 : 1. 创建root Pipeline(root_pipeline)

PipelineXFragmentContext -> PipelineXFragmentContext:_build_pipelines
note right of PipelineXFragmentContext #DAF7A6 : 2. 自上而下(先序遍历)创建Operator并构建Pipeline
note over of PipelineXFragmentContext #FF5733 : 1. Operator的init(_create_tree_helper)

PipelineXFragmentContext -> PipelineXFragmentContext:_create_data_sink
note over of PipelineXFragmentContext: 创建Fragment Output Sink\n(root_pipeline的sink Node)

PipelineXFragmentContext -> DataSinkOperatorX:init
note over of DataSinkOperatorX : init output sink

alt _enable_local_shuffle
PipelineXFragmentContext -> PipelineXFragmentContext:_plan_local_exchange
end

PipelineXFragmentContext -> Pipeline:prepare\n[遍历_pipelines对执行每个Pipeline的prepare]
note over of Pipeline #FF5733 : 2. 递归地Operator prepare和open

PipelineXFragmentContext -> PipelineXFragmentContext :_build_pipeline_tasks
note right of PipelineXFragmentContext #DAF7A6 : 3.构建PipelineTask
note right of PipelineXFragmentContext #8333FF : 1.构造函数创建share state(create_shared_state)\n2.在prepare阶段初始化local state(setup_local_state)

PipelineXFragmentContext -[#FF9F33]-> FragmentMgr
deactivate PipelineXFragmentContext

FragmentMgr -[#FF9F33]> PipelineXFragmentContext:submit
activate PipelineXFragmentContext #FF9F33
PipelineXFragmentContext -> TaskScheduler:schedule_task\n[遍历_tasks]
note left of TaskScheduler #FF5733 : 3. 含Operator的get_block_after_projects\n\tsink的sink
TaskScheduler --> PipelineXFragmentContext : Status
alt status OK
    alt eos
        TaskScheduler -> TaskScheduler:_try_close_task
        TaskScheduler -> PipelineTask:close
    end
else [异常]
    PipelineXFragmentContext -> PipelineXFragmentContext:cancel
    TaskScheduler -> TaskScheduler:_try_close_task
end
PipelineXFragmentContext -[#FF9F33]-> FragmentMgr
deactivate PipelineXFragmentContext
@enduml
```

```plantuml
@startuml
class PipelineXFragmentContext {
    - std::map<PipelineId, std::vector<PipelineId>> 
    - struct pipeline_parent_map _pipeline_parent_map
    - std::vector<std::vector<std::unique_ptr\n\t<PipelineXTask>>> _tasks
}
note top :  _dag通过PipelineId来管理Pipeline依赖\n_tasks是一个n*m的矩阵，在_build_pipeline_tasks中填充数据\n

class PipelineFragmentContext {
    - Pipelines _pipelines
    - std::map<PipelineId, std::vector<PipelineId>> _dag
    - int _num_instances = 1

    + PipelinePtr add_pipeline()
    + PipelinePtr add_pipeline(PipelinePtr parent, int idx)
}

struct pipeline_parent_map {
    + std::map<int, std::vector<PipelinePtr>> \n\t_build_side_pipelines
    + void push(int parent_node_id,\n\tPipelinePtr pipeline)
    + void pop(PipelinePtr& cur_pipe,\n\tint parent_node_id, int child_idx)
    + void clear()
}

class PipelineXTask {
    + Status execute(bool* eos) override
    + Status close(Status exec_status) override
}

class Pipeline {
    - OperatorXs operatorXs
    - DataSinkOperatorXPtr _sink_x

    - std::vector<std::pair<int, std::weak_ptr<\n\tPipeline>>> _parents
    - std::vector<std::pair<int, std::shared_ptr<\n\tPipeline>>> _dependencies

    - int _num_tasks = 1
    - int _num_tasks_created = 0

    + bool need_to_create_task() const

    + Status add_operator(OperatorXPtr& op)
    + Status set_sink(DataSinkOperatorXPtr& sink)

    + void add_dependency(std::shared_ptr<Pipeline>& pipeline)
    + void finish_one_dependency(int dep_opr,\n\tint dependency_core_id)
    + bool has_dependency()

    + Status prepare(RuntimeState* state)
}

PipelineXFragmentContext -left-|> PipelineFragmentContext
PipelineXTask -up-o PipelineXFragmentContext : 拥有一组PipelineXTask\n提交给TaskScheduler调度

pipeline_parent_map -left-* PipelineXFragmentContext
Pipeline -up-o PipelineFragmentContext

Pipeline -right--> PipelineXTask : 维护_num_tasks和\n_num_tasks_created\n决定是否创建PipelineXTask,\n如果需要创建\n在构造调用incr_created_tasks\n将_num_tasks_created自增1
@enduml
```



# PipelineXTask

PipelineXTask数据结构表达类图：
```plantuml
@startuml
class PipelineXTask {
    - std::vector<Dependency*> _read_dependencies
    - std::vector<Dependency*> _write_dependencies
    - std::vector<Dependency*> _finish_dependencies
    - std::vector<Dependency*> _filter_dependencies

    - OperatorXs _operators
    - OperatorXPtr _source
    - OperatorXPtr _root
    - DataSinkOperatorXPtr _sink

    + bool source_can_read() override
    + bool sink_can_write() override

    + Status execute(bool* eos) override
    + Status close(Status exec_status) override
}

class OperatorXBase {
    + virtual Status get_block_after_projects(RuntimeState* state,\n\tvectorized::Block* block, bool* eos)
}

PipelineXTask -down-|> PipelineTask
OperatorXBase -up-o PipelineXTask
@enduml
```


`PipelineXTask::prepare`中进行LocalState的初始化
```plantuml
@startuml
PipelineXTask -> DataSinkOperatorX:setup_local_state
DataSinkOperatorX -> LocalStateType:create_unique\n(this, RuntimeState)
DataSinkOperatorX -> LocalStateType:init
DataSinkOperatorX --> PipelineXTask
@enduml
```

# 参考资料
1. [Doris PipeplineX Execution Engine](https://cwiki.apache.org/confluence/display/DORIS/DSIP-035%3A+PipelineX+Execution+Engine)