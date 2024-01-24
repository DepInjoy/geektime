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

    + virtual Status setup_local_state(RuntimeState* state, LocalStateInfo& info) = 0
    + virtual DependencySPtr get_dependency(QueryContext* ctx) = 0
    + virtual DataDistribution required_data_distribution() const

    + void set_parallel_tasks(int parallel_tasks)
    + int parallel_tasks() const
}

class OperatorBase {
    # OperatorXPtr _child_x = nullptr
}

StreamingOperatorX -down-|> OperatorX
OperatorX -down-|> OperatorXBase
OperatorXBase -down-|> OperatorBase
@enduml
```
# 参考资料
1. [Doris PipeplineX Execution Engine](https://cwiki.apache.org/confluence/display/DORIS/DSIP-035%3A+PipelineX+Execution+Engine)