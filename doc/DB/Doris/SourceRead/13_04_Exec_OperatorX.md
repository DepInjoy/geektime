基于2.1.2-rc04分支

```plantuml
@startuml
TaskScheduler -> PipelineXTask:execute
activate PipelineXTask
PipelineXTask -> OperatorXBase:get_block_after_projects



@enduml
```
# Operator
算子的基本数据结构类图表示：
```plantuml
@startuml
class StreamingOperatorX {
    + Status get_block(RuntimeState* state,\n\t vectorized::Block* block, bool* eos) override
    + Status pull(RuntimeState* state,\n\tvectorized::Block* block, bool* eos) = 0
}
note top : pull执行模型

class StatefulOperatorX {
    + Status get_block(RuntimeState* state, vectorized::Block*\n\tblock,bool* eos) override
    + Status pull(RuntimeState* state, vectorized::Block*\n\tblock, bool* eos) const = 0;
    + Status push(RuntimeState* state, vectorized::Block*\n\tinput_block,bool eos) const = 0
    + bool need_more_input_data(RuntimeState* state) const = 0
}
note top : pull-push混合模型 

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

    + Status setup_local_state(RuntimeState* state,LocalStateInfo& info) = 0
    + DependencySPtr get_dependency(QueryContext* ctx) = 0
    + virtual DataDistribution required_data_distribution() const

    + void set_parallel_tasks(int parallel_tasks)
    + int parallel_tasks() const

    + Status get_block_after_projects(RuntimeState* state,\n\tvectorized::Block* block, bool* eos)
    + Status get_block(RuntimeState* state,\n\tvectorized::Block* block, bool* eos)
}
note bottom : 对外暴露get_block_after_projects接口\n内部实现主要调用get_block接口\nPipelineTask的execute调用该接口触发计算

class OperatorBase {
    # OperatorXPtr _child_x = nullptr

    + virtual size_t revocable_mem_size(RuntimeState* state) const
    + virtual Status revoke_memory(RuntimeState* state)
}

class ExchangeSinkOperatorX {
    - RuntimeState* _state
}

class DataSinkOperatorX {
    + std::shared_ptr<BasicSharedState>\n\tcreate_shared_state() const = 0
    + Status setup_local_state(RuntimeState* state,\n\t LocalSinkStateInfo& info)
    + LocalState& get_local_state(RuntimeState* state)

    + void get_dependency(std::vector<DependencySPtr>&\n\tdependency,QueryContext* ctx)
}

class DataSinkOperatorXBase {
    + virtual void get_dependency(std::vector<DependencySPtr>&\n\tdependency,QueryContext* ctx) = 0
    + virtual Status setup_local_state(RuntimeState* state,\n\tLocalSinkStateInfo& info) = 0;
    + virtual DataDistribution required_data_distribution() const
}

StreamingOperatorX -down-|> OperatorX
StatefulOperatorX -down-|> OperatorX

OperatorX -down-|> OperatorXBase
OperatorXBase -down-|> OperatorBase

DataSinkOperatorXBase -down-|> OperatorBase
DataSinkOperatorX -down-|> DataSinkOperatorXBase
ExchangeSinkOperatorX -down-|> DataSinkOperatorX
ResultSinkOperatorX -down-|> DataSinkOperatorX
@enduml
```


```C++
class PipelineXSinkLocalState : public PipelineXSinkLocalStateBase {
protected:
    DependencyType* _dependency = nullptr;
    typename DependencyType::SharedState* _shared_state = nullptr;
}

class PipelineXSpillSinkLocalState : public PipelineXSinkLocalState<SharedStateArg>

class AsyncWriterSink : public PipelineXSinkLocalState<FakeSharedState>
```

# 窗口
```plantuml
@startuml
class AnalyticSourceOperatorX {

}

class OperatorX {

    + Status setup_local_state(RuntimeState* state, LocalStateInfo& info) override;
    + LocalState& get_local_state(RuntimeState* state) const
}

class RuntimeState {
    - std::vector<std::unique_ptr<doris::pipeline::PipelineXLocalStateBase>>\n\t _op_id_to_local_state
    + void emplace_local_state(int id,std::unique_ptr<\n\tdoris::pipeline::PipelineXLocalStateBase> state)
    + doris::pipeline::PipelineXLocalStateBase get_local_state(int id)
}

AnalyticSourceOperatorX -down-|> OperatorX : AnalyticLocalState
OperatorX -down-> RuntimeState : 获取LocalState

@enduml
```

```plantuml
@startuml
class PipelineXLocalState {
    - Dependency* _dependency;
    - SharedStateArg* _shared_state

    + Status init(RuntimeState* state, LocalStateInfo& info) override
    + Status close(RuntimeState* state) override

    + std::vector<Dependency*> dependencies() const
}

interface PipelineXLocalStateBase {

}

struct AnalyticSharedState {
    + std::vector<vectorized::Block> input_blocks
    + 
}

struct BasicSharedState {
    + std::vector<DependencySPtr> source_deps;
    + std::vector<DependencySPtr> sink_deps;
    + int id = 0;
    + std::set<int> related_op_ids;

    + Dependency* create_source_dependency(int operator_id,\n\tint node_id, std::string name,QueryContext* ctx);
    + Dependency* create_sink_dependency(int dest_id,\n\tint node_id, std::string name, QueryContext* ctx);
}

AnalyticLocalState -down-|> PipelineXLocalState : SharedStateArg = AnalyticSharedState

AnalyticLocalState -right--> AnalyticSharedState
AnalyticSharedState -right-|> BasicSharedState

PipelineXLocalState -down-|> PipelineXLocalStateBase
@enduml
```

```plantuml
@startuml
class AnalyticSinkOperatorX {
    + Status sink(RuntimeState* state,vectorized::Block* in_block,bool eos)
    + DataDistribution required_data_distribution() const
}

class DataSinkOperatorX {
    + std::shared_ptr<BasicSharedState> create_shared_state() const = 0
}

class PipelineXSinkLocalState {
    # Dependency* _dependency = nullptr;
    # SharedStateType* _shared_state
}

AnalyticSinkOperatorX -down-|> DataSinkOperatorX : AnalyticSinkLocalState
AnalyticSinkOperatorX -right--> AnalyticSinkLocalState

AnalyticSinkLocalState -down-|> PipelineXSinkLocalState
PipelineXSinkLocalState -down-|> PipelineXSinkLocalStateBase
@enduml
```

# 集合运算
对于`INTERSECT`，`EXCEPT`运算

```plantuml
@startuml
class SetSinkLocalState {
    - vectorized::MutableBlock _mutable_block
    - vectorized::VExprContextSPtrs _child_exprs
    - vectorized::Arena _arena
}

class PipelineXSinkLocalState {
    # DataSinkOperatorXBase* _parent
    # RuntimeState* _state
    # bool _closed = false
    # std::shared_ptr<QueryStatistics> _query_statistics
}

struct SetSharedState {
    + std::vector<Dependency*> probe_finished_children_dependency
    + std::vector<vectorized::VExprContextSPtrs> probe_child_exprs_lists
}

SetSinkLocalState -down-|> PipelineXSinkLocalState
SetSinkLocalState -right--> SetSharedState

SetSharedState -down-|> BasicSharedState
@enduml
```

# TableFunction
```plantuml
@startuml
class TableFunctionOperatorX {
    + Status push(RuntimeState* state, \n\tvectorized::Block* input_block, bool eos) const
    + Status pull(RuntimeState* state,\n\tvectorized::Block* output_block, bool* eos) const
}
note top : StatefulOperator的一种，pull-push混合模型

class TableFunctionLocalState {
    + void process_next_child_row()
    + Status get_expanded_block(RuntimeState* state,\n\t vectorized::Block* output_block, bool* eos)
}

class StatefulOperatorX {
    + Status get_block(RuntimeState* state, vectorized::Block*\n\tblock,bool* eos) override
    + Status pull(RuntimeState* state, vectorized::Block*\n\tblock, bool* eos) const = 0;
    + Status push(RuntimeState* state, vectorized::Block*\n\tinput_block,bool eos) const = 0
    + bool need_more_input_data(RuntimeState* state) const = 0
}

TableFunctionOperatorX -down-|> StatefulOperatorX

TableFunctionLocalState -left--> TableFunctionOperatorX
TableFunctionLocalState -down-|> PipelineXLocalState
@enduml
```

## Groupping
```plantuml
@startuml
class RepeatOperatorX {
    + bool need_more_input_data(RuntimeState* state)const
    + Status pull(RuntimeState* state, vectorized::Block*\n\toutput_block, bool* eos) const
    + Status push(RuntimeState* state, vectorized::Block*\n\tinput_block, bool eos) const
}
note top : Groupping计算，含groupping set, rollup和cube

class RepeatLocalState {
    + Status get_repeated_block(vectorized::Block* child_block,\n\tint repeat_id_idx, vectorized::Block* output_block)
    + Status add_grouping_id_column(std::size_t rows,\n\tstd::size_t& cur_col,vectorized::MutableColumns&\n\tcolumns, int repeat_id_idx);

}

RepeatOperatorX -down-|> StatefulOperatorX
RepeatLocalState -left--> RepeatOperatorX 

@enduml
```