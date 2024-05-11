# 设计
从对应关系上分为四类：

1对1dependency：一个上游pipeline task的dependency和一个下游pipeline task的dependency之间通过一个建立阻塞/运行关系，典型例子：一个AggSourceDependency对应一个AggSinkDependency
<center>
    <img src="https://cwiki.apache.org/confluence/download/attachments/288885193/d1.png?version=1&modificationDate=1705484609000&api=v2" height=50% width=50%>
</center>
1对多dependency：一个上游pipeline task的dependency和多个下游pipeline task的dependency之间通过多个dependency建立阻塞/运行关系，典型例子：一个MultiCastSinkDependency对应多个MultiCastSourceDependency
<center>
    <img src="https://cwiki.apache.org/confluence/download/attachments/288885193/d2.png?version=1&modificationDate=1705484610000&api=v2" height=50% width=50%>
</center>

多对1dependency：多个上游pipeline task和1个下游pipeline task之间通过多个dependency建立阻塞/运行关系，典型例子：多个UnionSinkDependency/SetSinkDependency对应一个UnionSourceDependency/SetSourceDependency
<center>
    <img src="https://cwiki.apache.org/confluence/download/attachments/288885193/d3.png?version=1&modificationDate=1705484611000&api=v2" height=50% width=50%>
</center>

多对多dependency：多个上游pipeline task和多个下游pipeline task之间通过多个dependency建立阻塞/运行关系，典型例子：LocalExchangeSourceDependency/LocalExchangeSinkDependency
<center>
    <img src="https://cwiki.apache.org/confluence/download/attachments/288885193/d4.png?version=1&modificationDate=1705484613000&api=v2" height=50% width=50%>
</center>

# 实现

```plantuml
@startuml
class PipelineXLocalState {
    # Dependency* _dependency
    # SharedStateArg* _shared_state
}
note top : 持有source dependency,参见init接口调用\n_shared_state->create_source_dependency

class PipelineXSinkLocalState {
    # Dependency* _dependency
    # SharedStateType* _shared_state
}
note top : 持有的sink dependency参见init接口调用\n _shared_state->create_sink_dependency

class Dependency {
    - BasicSharedState* _shared_state
}

PipelineXLocalState -down-o Dependency
PipelineXSinkLocalState -down-o Dependency
Dependency -right-o BasicSharedState
@enduml
```

```plantuml
@startuml


class Dependency {
    - std::vector<PipelineXTask*> _blocked_task
    - BasicSharedState* _shared_state
    + void block()
    + void set_ready()
}
note bottom : set_ready通知下游的Task dependency就绪\n将blocked task wakeup(重新放入执行队列)

class BasicSharedState {
    - std::vector<DependencySPtr> source_deps
    - std::vector<DependencySPtr> sink_deps
    - int id = 0
    - std::set<int> related_op_ids

    + Dependency* create_source_dependency(int operator_id,\n\tint node_id, std::string name,QueryContext* ctx)
    Dependency* create_sink_dependency(int dest_id,\n\tint node_id, std::string name,QueryContext* ctx);
}

class PipelineXTask {
    - TaskQueue* _task_queue
    + void wake_up()
}
note top : dependency就绪后\n将task push到TaskQueue

PipelineXTask -down-o Dependency
BasicSharedState -left-- Dependency

@enduml
```
```C++
PipelineXTask::PipelineXTask(PipelinePtr& pipeline, ......)
        : PipelineTask(pipeline, task_id, state, fragment_context, parent_profile),
          _source(pipeline->source_shared_pointer()),   
          _sink(pipeline->sink_shared_pointer()), ......) {
            ......
    // 创建shared state
    auto shared_state = _sink->create_shared_state();
    if (shared_state) {
        _sink_shared_state = shared_state;
    }

    // 创建的Pipeline Task(_num_tasks_created)自增1
    pipeline->incr_created_tasks();
}
```
```C++
Status PipelineXTask::prepare(const TPipelineInstanceParams& local_params,
        const TDataSink& tsink, QueryContext* query_ctx) {
    // 1. setup sink local state
    {
        LocalSinkStateInfo info {_task_idx, _task_profile.get(),
            local_params.sender_id, get_sink_shared_state().get(),
            _le_state_map, tsink};
        RETURN_IF_ERROR(_sink->setup_local_state(_state, info));
    }

    std::vector<TScanRangeParams> no_scan_ranges;
    auto scan_ranges = find_with_default(local_params.per_node_scan_ranges,
            _operators.front()->node_id(), no_scan_ranges);
    auto* parent_profile = _state->get_sink_local_state()->profile();
    query_ctx->register_query_statistics(
            _state->get_sink_local_state()->get_query_statistics_ptr());

    for (int op_idx = _operators.size() - 1; op_idx >= 0; op_idx--) {
        auto& op = _operators[op_idx];
        LocalStateInfo info {parent_profile, scan_ranges,
                get_op_shared_state(op->operator_id()),
                _le_state_map, _task_idx};
        RETURN_IF_ERROR(op->setup_local_state(_state, info));
        parent_profile = _state->get_local_state(op->operator_id())->profile();
        query_ctx->register_query_statistics(
                _state->get_local_state(op->operator_id())->get_query_statistics_ptr());
    }

    _block = doris::vectorized::Block::create_unique();
    RETURN_IF_ERROR(_extract_dependencies());
    set_state(PipelineTaskState::RUNNABLE);
    _prepared = true;
    return Status::OK();
}
```
`PipelineXTask::prepare`
```plantuml
@startuml

PipelineXTask -> DataSinkOperatorX:setup_local_state
activate PipelineXTask

activate DataSinkOperatorX
DataSinkOperatorX ->LocalStateType:init
LocalStateType -> PipelineXSinkLocalState:init
PipelineXSinkLocalState -> BasicSharedState:create_sink_dependency
LocalStateType -->DataSinkOperatorX
deactivate DataSinkOperatorX

DataSinkOperatorX --> PipelineXTask
deactivate PipelineXTask
@enduml
```

```plantuml
@startuml
PipelineXTask -> OperatorX:setup_local_state
activate PipelineXTask
OperatorX -> LocalStateType:init

activate OperatorX
LocalStateType -> PipelineXLocalState:init
PipelineXLocalState -> BasicSharedState:create_source_dependency
LocalStateType --> OperatorX
deactivate OperatorX

OperatorX -->PipelineXTask
deactivate PipelineXTask
@enduml
```
PipelineX如何实现多个pipeline task共享同一个pipeline的全部共享状态，例如表达式和一些const变量，消除了额外的初始化开销。

```C++
struct AnalyticSharedState : public BasicSharedState {
public:
    AnalyticSharedState() = default;

    int64_t current_row_position = 0;
    vectorized::BlockRowPos partition_by_end;
    vectorized::VExprContextSPtrs partition_by_eq_expr_ctxs;
    int64_t input_total_rows = 0;
    vectorized::BlockRowPos all_block_end;
    std::vector<vectorized::Block> input_blocks;
    bool input_eos = false;
    vectorized::BlockRowPos found_partition_end;
    std::vector<int64_t> origin_cols;
    vectorized::VExprContextSPtrs order_by_eq_expr_ctxs;
    std::vector<int64_t> input_block_first_row_positions;
    std::vector<std::vector<vectorized::MutableColumnPtr>> agg_input_columns;


    std::vector<int64_t> partition_by_column_idxs;
    std::vector<int64_t> ordey_by_column_idxs;
};
```