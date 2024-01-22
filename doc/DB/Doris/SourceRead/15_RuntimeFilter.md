
```plantuml
@startuml
class RuntimeState {
    - std::unique_ptr<RuntimeFilterMgr> _runtime_filter_mgr
}

class RuntimeFilterMgr {
    - std::map<int32_t, std::vector<ConsumerFilterHolder>> _consumer_map
    - std::map<int32_t, IRuntimeFilter*> _producer_map
    - RuntimeState* _state
    - std::unique_ptr<MemTracker> _tracker
    + Status get_consume_filter(const int filter_id, const int node_id,\n\tIRuntimeFilter** consumer_filter)
    + Status get_consume_filters(const int filter_id,\n\tstd::vector<IRuntimeFilter*>& consumer_filters)
    + Status get_producer_filter(const int filter_id,\n\tIRuntimeFilter** producer_filter)

    + Status register_consumer_filter(const TRuntimeFilterDesc& desc,\n\tconst TQueryOptions& options, int node_id,\n\tbool build_bf_exactly = false);
    + Status register_producer_filter(const TRuntimeFilterDesc& desc,\n\tconst TQueryOptions& options, bool build_bf_exactly = false)
}
note left: comsumer和producer map的key是filter-id

struct ConsumerFilterHolder {
    + int node_id;
    + IRuntimeFilter* filter;
}

RuntimeFilterMgr <-down-> RuntimeState
ConsumerFilterHolder -down-o RuntimeFilterMgr
IRuntimeFilter -down-o RuntimeFilterMgr
@enduml
```

```C++
Status PipelineFragmentContext::prepare(
    const doris::TPipelineFragmentParams& request, const size_t idx) {
            .....
    // 1. init _runtime_state
    _runtime_state = RuntimeState::create_unique(...);
    _runtime_state->runtime_filter_mgr()->init();
            .....
}
```