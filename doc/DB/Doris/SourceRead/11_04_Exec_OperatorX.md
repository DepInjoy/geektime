基于2.1.2-rc04分支

```plantuml
@startuml
class DataSinkOperatorX {
    + std::shared_ptr<BasicSharedState> create_shared_state() const = 0;
    + virtual DataDistribution required_data_distribution() const
    + Status setup_local_state(RuntimeState* state, LocalSinkStateInfo& info) = 0
}

class DataSinkOperatorXBase {
    + Status setup_local_state(RuntimeState* state, \n\tLocalSinkStateInfo& info) = 0
    + void get_dependency(std::vector<DependencySPtr>& \n\tdependency,QueryContext* ctx) = 0
    + virtual DataDistribution required_data_distribution() const
}

class OperatorXBase {
    + DataDistribution required_data_distribution() const
    + DependencySPtr get_dependency(QueryContext* ctx) = 0
    + Status get_block_after_projects(RuntimeState* state, \n\tvectorized::Block* block, bool* eos)
}

class OperatorBase {
    + virtual size_t revocable_mem_size(RuntimeState* state) const { return 0; }
    + virtual Status revoke_memory(RuntimeState* state)
}

class OperatorX {
    + LocalState& get_local_state(RuntimeState* state) const 
}
DataSinkOperatorX -down-|> DataSinkOperatorXBase
DataSinkOperatorXBase -down-|> OperatorBase

OperatorX -down-|>OperatorXBase
OperatorXBase -down-|> OperatorBase


@enduml
```

```C++
class PipelineXSinkLocalState : public PipelineXSinkLocalStateBase {
protected:
    DependencyType* _dependency = nullptr;
    typename DependencyType::SharedState* _shared_state = nullptr;
}
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
    + std::vector<vectorized::VExprContextSPtrs> probe_child_exprs_lists;
}

SetSinkLocalState -down-|> PipelineXSinkLocalState
SetSinkLocalState --> SetSharedState

SetSharedState -down-|> BasicSharedState
@enduml
```