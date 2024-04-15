# 接口变更和使用
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
note over of Pipeline: 初始化global state
note over of Pipeline #FF5733 : 2. 递归地Operator prepare和open

PipelineXFragmentContext -> PipelineXFragmentContext :_build_pipeline_tasks
note right of PipelineXFragmentContext #DAF7A6 : 3.构建PipelineTask并初始化local state

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
    - - std::vector<std::vector<std::unique_ptr<PipelineXTask>>> _tasks
}
note top :  _dag通过PipelineId来管理Pipeline依赖\n_tasks是一个n*m的矩阵，在_build_pipeline_tasks中填充数据\n

class PipelineFragmentContext {
    - Pipelines _pipelines
}

struct pipeline_parent_map {
    + std::map<int, std::vector<PipelinePtr>> _build_side_pipelines
    + void push(int parent_node_id, PipelinePtr pipeline)
    + void pop(PipelinePtr& cur_pipe, int parent_node_id, int child_idx)
    + void clear()
}

class Pipeline {
    + Status add_operator(OperatorXPtr& op)
    + Status set_sink(DataSinkOperatorXPtr& sink)

    + void add_dependency(std::shared_ptr<Pipeline>& pipeline)
    + void finish_one_dependency(int dep_opr, int dependency_core_id)
    + bool has_dependency()

    + Status prepare(RuntimeState* state)

    - OperatorXs operatorXs
    - DataSinkOperatorXPtr _sink_x

    - std::vector<std::pair<int, std::weak_ptr<Pipeline>>> _parents
    - std::vector<std::pair<int, std::shared_ptr<Pipeline>>> _dependencies
}

PipelineXFragmentContext -down-|> PipelineFragmentContext
pipeline_parent_map -left-* PipelineXFragmentContext
Pipeline -up-o PipelineFragmentContext
@enduml
```

# Operator

```plantuml
@startuml
class StreamingOperatorX {

}

class OperatorX {
    + Status setup_local_state(RuntimeState* state, LocalStateInfo& info)
    + LocalState& get_local_state(RuntimeState* state) const
    + DependencySPtr get_dependency(QueryContext* ctx)
}

class OperatorXBase {
    # const int _operator_id
    # const int _node_id
    # TPlanNodeType::type _type;
    # int _parallel_tasks
    # std::unique_ptr<RowDescriptor> _output_row_descriptor(nullptr)

    + virtual Status setup_local_state(RuntimeState* state,\n\tLocalStateInfo& info) = 0
    + virtual DependencySPtr get_dependency(QueryContext* ctx) = 0
    + virtual DataDistribution required_data_distribution() const

    + void set_parallel_tasks(int parallel_tasks)
    + int parallel_tasks() const
}

class OperatorBase {
    # OperatorXPtr _child_x = nullptr
}

class ExchangeSinkOperatorX {
    - RuntimeState* _state
}

class DataSinkOperatorX {
    + Status setup_local_state(RuntimeState* state, LocalSinkStateInfo& info)
    + void get_dependency(std::vector<DependencySPtr>& dependency,\n\tQueryContext* ctx)
    + LocalState& get_local_state(RuntimeState* state)
}

class DataSinkOperatorXBase {
    + virtual void get_dependency(\n\tstd::vector<DependencySPtr>& dependency,\n\tQueryContext* ctx) = 0
    + virtual Status setup_local_state(RuntimeState* state,\n\tLocalSinkStateInfo& info) = 0;
}

StreamingOperatorX -down-|> OperatorX
OperatorX -down-|> OperatorXBase
OperatorXBase -down-|> OperatorBase

DataSinkOperatorXBase -down-|> OperatorBase
DataSinkOperatorX -down-|> DataSinkOperatorXBase
ExchangeSinkOperatorX -down-|> DataSinkOperatorX
ResultSinkOperatorX -down-|> DataSinkOperatorX
@enduml
```
# 参考资料
1. [Doris PipeplineX Execution Engine](https://cwiki.apache.org/confluence/display/DORIS/DSIP-035%3A+PipelineX+Execution+Engine)