火山模型的调用入口是
```C++
FragmentMgr::exec_plan_fragment(const TExecPlanFragmentParams& params)
```

借助`FragmentExecState`完成整个生命周期调用，它拥有一个`PlanFragmentExecutor`，通过它实现：
1. `PlanFragmentExecutor::prepare`: 创建Exec Tree并init，参见`ExecNode::create_tree`；递归地执行Operator的`prepare`
2. `PlanFragmentExecutor::open`:执行算子的`open`和`get_next`
3. `PlanFragmentExecutor::close`:执行算子的`close`

```plantuml
@startuml
class FragmentExecState {
    - PlanFragmentExecutor _executor

    + Status prepare(const TExecPlanFragmentParams& params)
    + Status execute()
}

class PlanFragmentExecutor {
    - ExecNode* _plan

    + Status prepare(const TExecPlanFragmentParams& request, \n\tQueryContext* query_ctx = nullptr)
    + Status open()
    + void close()

    - Status open_vectorized_internal()
    - Status get_vectorized_internal(::doris::vectorized::Block* block, bool* eos)
}

PlanFragmentExecutor -up-o FragmentExecState
@enduml
```

# 主流程图
```plantuml
@startuml
PInternalServiceImpl -> FragmentMgr:exec_plan_fragment
group exec_plan_fragment
FragmentMgr -> FragmentMgr:_get_query_ctx
note left of FragmentMgr: 创建QueryContext

FragmentMgr -> FragmentExecState:new

FragmentMgr -[#FF9F33]> FragmentExecState:prepare
activate FragmentMgr #FF9F33
FragmentExecState -[#FF5733]> PlanFragmentExecutor:prepare
note left of PlanFragmentExecutor #FF5733: 1. 创建Exec树\n2. 调用Operator的init和prepare
FragmentExecState --> FragmentMgr : Status
deactivate FragmentMgr


FragmentMgr -> ThreadPool:submit_func
note left of ThreadPool: 提交的func是_exec_actual\n含Operator的Open,get_next和close

    group FragmentMgr::_exec_actual
        FragmentMgr -[#FF9F33]> FragmentExecState:execute
        activate FragmentMgr #FF9F33
        FragmentExecState -[#FF5733]> PlanFragmentExecutor:open
        activate FragmentExecState
        note left of PlanFragmentExecutor #FF5733: 3. open和get_next取block
        PlanFragmentExecutor --> FragmentExecState : Status
        deactivate FragmentExecState
        alt 成功
            FragmentExecState -[#FF5733]> PlanFragmentExecutor:close
            activate FragmentExecState
            note left of PlanFragmentExecutor #FF5733 : 4. 递归地调用Operator的close
            deactivate FragmentExecState
        else 失败
            FragmentExecState -> FragmentExecState:cancel
            activate FragmentExecState
            FragmentExecState -> PlanFragmentExecutor:cancel
            PlanFragmentExecutor -> RuntimeState:set_is_cancelled
            PlanFragmentExecutor -> VDataStreamMgr:cancel(fragment_instance_id)
            note left of VDataStreamMgr: cancle所有fragment的stream
            PlanFragmentExecutor -> ResultQueueMgr:update_queue_status
            note left of ResultQueueMgr: Status::Aborted
            deactivate FragmentExecState
    end group
    FragmentExecState --> FragmentMgr : Status
    deactivate FragmentMgr
end group
@enduml
```

## Executor::prepare
```plantuml
@startuml
FragmentExecState -> PlanFragmentExecutor:prepare
PlanFragmentExecutor -> RuntimeState:create_unique
PlanFragmentExecutor -> DescriptorTbl:create

PlanFragmentExecutor -> ExecNode:create_tree
activate PlanFragmentExecutor
note left of ExecNode : 1. 建立Exec Tree并进行init\n\t_plan表示Exec Tree的根节点
deactivate PlanFragmentExecutor

PlanFragmentExecutor -> ExecNode:prepare
note left of ExecNode: 2. 递归地执行prepare
@enduml
```
`ExecNode:create_tree`参见[这里的 创建Exec树 章节](11_00_00_Exec_CreateTree.md)

## Executor::open
```plantuml
@startuml
FragmentExecState -> PlanFragmentExecutor:open
PlanFragmentExecutor -> PlanFragmentExecutor:open_vectorized_internal
group open_vectorized_internal
    PlanFragmentExecutor -> ExecNode:open
    activate PlanFragmentExecutor
    note right of PlanFragmentExecutor : 1. 从根节点_plan，递归递执行open
    PlanFragmentExecutor -> DataSink:open
    PlanFragmentExecutor -> PlanFragmentExecutor:get_vectorized_internal
    note right of PlanFragmentExecutor: 2. 调用跟_plan的get_next\n每个ExecNode调用孩子的get_next实现
    PlanFragmentExecutor -> DataSink:send
    note left of DataSink: 取到数据
    deactivate PlanFragmentExecutor
end group
@enduml
```

对于`PlanFragmentExecutor::get_vectorized_internal`
```plantuml
@startuml
PlanFragmentExecutor -> ExecNode:get_next_after_projects
group ExecNode::get_next_after_projects
alt _output_row_descriptor
    ExecNode -> ExecNode:get_next
    ExecNode -> ExecNode:do_projections
else
ExecNode -> ExecNode:get_next
end group
@enduml
```

```C++
Status ExecNode::get_next_after_projects(
        RuntimeState* state, vectorized::Block* block, bool* eos,
        const std::function<Status(RuntimeState*, vectorized::Block*, bool*)>& func,
        bool clear_data) {
    if (_output_row_descriptor) {
        if (clear_data) {
            clear_origin_block();
        }
        auto status = func(state, &_origin_block, eos);
        if (UNLIKELY(!status.ok())) return status;
        return do_projections(&_origin_block, block);
    }
    _peak_memory_usage_counter->set(_mem_tracker->peak_consumption());
    return func(state, block, eos);
}
```

以`VSelectNode`为例
```C++
Status VSelectNode::get_next(RuntimeState* state, vectorized::Block* block, bool* eos) {
    SCOPED_TIMER(_runtime_profile->total_time_counter());
    RETURN_IF_CANCELLED(state);
    do {
        RETURN_IF_CANCELLED(state);
        // 调用孩子的get_next
        RETURN_IF_ERROR(_children[0]->get_next_after_projects(
            state, block, &_child_eos, std::bind((Status(ExecNode::*)
            (RuntimeState*, vectorized::Block*, bool*)) &ExecNode::get_next,
                _children[0], std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)));
        if (_child_eos) {
            *eos = true;
            break;
        }
    } while (block->rows() == 0);

    return pull(state, block, eos);
}
```
# Executor::close
```plantuml
@startuml
FragmentExecState -> PlanFragmentExecutor:close
PlanFragmentExecutor -> ExecNode:close
note left of ExecNode: 递归地调用Operator的close\n\t从跟节点_plan开始执行close\n\t并执行子节点的close

PlanFragmentExecutor -> DataSink:close
note left of DataSink: 执行sink的close
@enduml
```

```C++
void PlanFragmentExecutor::close() {
    if (_closed) return;

    // 执行了prepare
    if (_runtime_state != nullptr) {
        // _runtime_state init failed
        if (_plan != nullptr) {
            _plan->close(_runtime_state.get());
        }

        if (_sink != nullptr) {
            if (_prepared) {
                Status status;
                {
                    std::lock_guard<std::mutex> l(_status_lock);
                    status = _status;
                }
                _sink->close(runtime_state(), status);
            } else {
                _sink->close(runtime_state(), Status::InternalError("prepare failed"));
            }
        }
    }

    // 如果没有prepare,直接close
    _closed = true;
}
```

```C++
Status ExecNode::close(RuntimeState* state) {
    if (_is_closed) {
        return Status::OK();
    }

    _is_closed = true;
    Status result;
    // 执行子节点的close
    for (int i = 0; i < _children.size(); ++i) {
        auto st = _children[i]->close(state);
        if (result.ok() && !st.ok()) {
            result = st;
        }
    }

    release_resource(state);
    return result;
}
```

# 后端相关配置参数
```C++
config::fragment_pool_thread_num_min
config::fragment_pool_thread_num_max
config::fragment_pool_queue_size

FragmentMgr::FragmentMgr(ExecEnv* exec_env){
    s = ThreadPoolBuilder("FragmentMgrThreadPool")
                .set_min_threads(config::fragment_pool_thread_num_min)
                .set_max_threads(config::fragment_pool_thread_num_max)
                .set_max_queue_size(config::fragment_pool_queue_size)
                .build(&_thread_pool);
```
