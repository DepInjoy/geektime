# 窗口
前端窗口表示的数据结构是`public AnalyticEvalNode`
```java
// org/apache/doris/planner/AnalyticEvalNode.java
public AnalyticEvalNode {

}
```

## 数据结构

```plantuml
@startuml
class VAnalyticEvalNode {
    - TAnalyticWindow _window
    - AnalyticFnScope _fn_scope
    - int64_t _rows_start_offset = 0
    - int64_t _rows_end_offset = 0
    - std::vector<AggFnEvaluator*> _agg_functions
    - std::vector<VExprContextSPtrs> _agg_expr_ctxs
    - VExprContextSPtrs _partition_by_eq_expr_ctxs
    - VExprContextSPtrs _order_by_eq_expr_ctxs
    - std::vector<int64_t> _ordey_by_column_idxs
    - std::vector<int64_t> _partition_by_column_idxs
}

class TAnalyticNode {
    + TAnalyticWindow window
    + std::vector< ::doris::TExpr>  analytic_functions
    + std::vector< ::doris::TExpr>  partition_exprs
    + std::vector< ::doris::TExpr>  order_by_exprs
    + doris::TExpr partition_by_eq;
    + doris::TExpr order_by_eq;
}
note bottom: 前端发送过来计划的Thrift结构

class TAnalyticWindow {
    - TAnalyticWindowType::type type
    - TAnalyticWindowBoundary window_start
    - TAnalyticWindowBoundary window_end
}

TAnalyticWindow -up-o VAnalyticEvalNode : 保存TAnalyticNode::window
TAnalyticNode -left-- VAnalyticEvalNode
@enduml
```

```plantuml
@startuml
class VAnalyticEvalNode {
    - TAnalyticWindow _window
    - AnalyticFnScope _fn_scope
    - std::vector<VExprContextSPtrs> _agg_expr_ctxs
    - std::vector<AggFnEvaluator*> _agg_functions
    - std::unique_ptr<Arena> _agg_arena_pool
    - std::vector<std::vector<MutableColumnPtr>> _agg_intput_columns
    - executor _executor
}

class ExecNode {
    - int64_t _limit; // -1: no limit
    - int64_t _num_rows_returned
    # vectorized::VExprContextSPtrs _conjuncts
    # std::vector<ExecNode*> _children

    + void reached_limit(vectorized::Block* block,\n\tbool* eos)
}

class AggFnEvaluator {
    - AggregateFunctionPtr _function

    + Status create(ObjectPool* pool,const TExpr& desc,\n\tconst TSortInfo& sort_info, AggFnEvaluator** result)

    + void create(AggregateDataPtr place)
    + void destroy(AggregateDataPtr place)

    + void insert_result_info(AggregateDataPtr place, IColumn* column)
}

struct executor {
    + vectorized_execute execute
    + vectorized_get_next get_next
    + vectorized_get_result insert_result
    + vectorized_closer close
}
note bottom:get_next分Partition,Row和Range\n在构造为其设置不同函数\n在prepare进行executor设置insert_result和execute\n分别为_insert_result_infoh和_execute_for_win_func

interface IAggregateFunction {
    + size_t align_of_data() const = 0
    + void reset(AggregateDataPtr place) const = 0
    + void add_range_single_place(int64_t partition_start,\n\tint64_t partition_end, int64_t frame_start,\n\tint64_t frame_end,AggregateDataPtr place,\n\tconst IColumn** columns,Arena* arena) const = 0
    + void insert_result_into(ConstAggregateDataPtr __restrict place,\n\tIColumn& to) const = 0
}
note bottom : 窗口函数通过add_range_single_place计算

VAnalyticEvalNode -left-|> ExecNode

AggFnEvaluator -up-o VAnalyticEvalNode
IAggregateFunction -up-> AggFnEvaluator
IAggregateFunction -right--> AggregateFunctionSimpleFactory : 生成窗口执行函数
executor-left--> VAnalyticEvalNode
@enduml
```

窗口函数相关的生成函数在`be/src/vec/aggregate_functions/aggregate_function_window.cpp`

### 值函数

```plantuml
@startuml
class AggregateFunctionSimpleFactory {
    - AggregateFunctions aggregate_functions;
    - AggregateFunctions nullable_aggregate_functions;
    - std::unordered_map<std::string, std::string> function_alias;
    + void register_function_both(const std::string& name,\n\tconst Creator& creator)
}

struct LeadLagData {
    + void set_value_from_default()
    + void check_default(const IColumn* column)
}

struct WindowFunctionLagImpl {
    + void add_range_single_place(\n\tint64_t partition_start,\n\tint64_t partition_end, \n\tint64_t frame_start,\n\tint64_t frame_end,\n\tconst IColumn** columns)
}

WindowFunctionLastImpl -up-o AggregateFunctionSimpleFactory
WindowFunctionFirstImpl -up-o AggregateFunctionSimpleFactory
WindowFunctionLagImpl -up-o AggregateFunctionSimpleFactory
WindowFunctionLeadImpl -up-o AggregateFunctionSimpleFactory


WindowFunctionLeadImpl -down-|> LeadLagData
WindowFunctionLagImpl -down-|> LeadLagData
WindowFunctionLastImpl -down-|> FirstLastData
WindowFunctionFirstImpl -down-|> FirstLastData
@enduml
```



### TopN函数

```C++
void register_aggregate_function_window_rank(AggregateFunctionSimpleFactory& factory) {
    factory.register_function("dense_rank", creator_without_type::creator<WindowFunctionDenseRank>);
    factory.register_function("rank", creator_without_type::creator<WindowFunctionRank>);
    factory.register_function("row_number", creator_without_type::creator<WindowFunctionRowNumber>);
    factory.register_function("ntile", creator_without_type::creator<WindowFunctionNTile>);
}
```

```plantuml
@startuml
WindowFunctionDenseRank -up-o AggregateFunctionSimpleFactory
WindowFunctionRank -up-o AggregateFunctionSimpleFactory
WindowFunctionRowNumber -up-o AggregateFunctionSimpleFactory
WindowFunctionNTile -up-o AggregateFunctionSimpleFactory
@enduml
```

## 接口实现

这里[记录了一些窗口定义中成员](11_04_00_Exec_Operator_Analytic.md)，辅助理解代码。

### init和prepare
```plantuml
@startuml

fragmentCtx -> VAnalyticEvalNode : init
activate VAnalyticEvalNode
VAnalyticEvalNode -> VExpr:create_tree_from_thrift
note over of VExpr: 创建Expr树\n将其push到对应的_agg_expr_ctxs

VAnalyticEvalNode -> AggFnEvaluator:create
note over of AggFnEvaluator : 将其push到_agg_functions

VAnalyticEvalNode --> fragmentCtx
deactivate VAnalyticEvalNode

fragmentCtx -> VAnalyticEvalNode:prepare
activate VAnalyticEvalNode
VAnalyticEvalNode -> AggFnEvaluator:prepare\n[配置]
activate AggFnEvaluator
    loop _agg_functions
    AggFnEvaluator -> AggregateFunctionSimpleFactory:instance[单例,注册函数]
    AggFnEvaluator -> AggregateFunctionSimpleFactory:get[获取_function]
    end
deactivate AggFnEvaluator
VAnalyticEvalNode -> Arena:aligned_alloc [内存对齐方式为结果列申请内存]

VAnalyticEvalNode -> VAnalyticEvalNode:_create_agg_status
loop _agg_functions
    VAnalyticEvalNode -> AggFnEvaluator:create(AggregateDataPtr)
    AggFnEvaluator -> _function:create
end

VAnalyticEvalNode -> VExpr:prepare
note over of VExpr: 含_agg_expr_ctxs\n_partition_by_eq_expr_ctxs\n_order_by_eq_expr_ctxs
VAnalyticEvalNode --> fragmentCtx
deactivate VAnalyticEvalNode
@enduml
```

### open

```plantuml
@startuml
fragmentCtx -> VAnalyticEvalNode:open
activate VAnalyticEvalNode
VAnalyticEvalNode -> VAnalyticEvalNode:alloc_resource
group VAnalyticEvalNode::alloc_resource
VAnalyticEvalNode -> ExecNode:alloc_resource
VAnalyticEvalNode -> VExpr:open
note over of VExpr: 含_agg_expr_ctxs\n_partition_by_eq_expr_ctxs\n_order_by_eq_expr_ctxs
loop _agg_functions
    VAnalyticEvalNode -> AggFnEvaluator:open
end
end

VAnalyticEvalNode -> child0:open
note over of child0: 递归执行孩子的open

VAnalyticEvalNode --> fragmentCtx
deactivate VAnalyticEvalNode
@enduml
```



### get_next



| `executor`结构            | 对应函数                                     | 何时设置  | 描述                                 |
| ------------------------- | -------------------------------------------- | --------- | ------------------------------------ |
| `_executor.get_next`      | `VAnalyticEvalNode::_get_next_for_partition` | 构造      | `PARTITION`类型获取Partition并写结果 |
|                           | `VAnalyticEvalNode::_get_next_for_range`     | 构造      | `RANGE`类型的获取Partition并写结果   |
|                           | `VAnalyticEvalNode::_get_next_for_rows`      | 构造      | `ROWS`类型的获取Partition并写结果    |
| `_executor.execute`       | `VAnalyticEvalNode::_execute_for_win_func`   | `prepare` | 计算Partition的结果                  |
| `_executor.insert_result` | `VAnalyticEvalNode::_insert_result_info`     | `prepare` | 写结果                               |
|                           |                                              |           |                                      |



```plantuml
@startuml
fragmentCtx -> VAnalyticEvalNode:get_next
activate VAnalyticEvalNode
loop !_input_eos || _output_block_index < _input_blocks.size()
    VAnalyticEvalNode -> VAnalyticEvalNode:_consumed_block_and_init_partition
    note over of VAnalyticEvalNode: 计算partition end,根据需要拉取分区\n表达式计算并初始化窗口结果列

    VAnalyticEvalNode -> executor:get_next
    note over of executor : 根据Frame类型计算并插入结果\n含execute和insert_result(实际执行接口见上方表格)
end

VAnalyticEvalNode -> VAnalyticEvalNode:_output_current_block
VAnalyticEvalNode -> VExprContext:

VAnalyticEvalNode -> ExecNode:reached_limit
VAnalyticEvalNode --> fragmentCtx
deactivate VAnalyticEvalNode
@enduml
```

```C++
Status VAnalyticEvalNode::_consumed_block_and_init_partition(RuntimeState* state,
                                                             bool* next_partition, bool* eos) {
    // 1. 二分法计算当前的partition end(block_num和row num)
    BlockRowPos found_partition_end = _get_partition_by_end();

    // 2. 检查是否需要获取下一个分区
    //    (if current partition haven't execute done, return false)
    while (whether_need_next_partition(found_partition_end)) {
        // 2.1 获取下一个block并计算Function表达式(sink接口,expr->execute)
        //     更新partition和order表达式在block的column index
        //     (_ordey_by_column_idxs, _partition_by_column_idxs)
        RETURN_IF_ERROR(_fetch_next_block_data(state));

        // 2.2 重新计算partition end
        found_partition_end = _get_partition_by_end();
    }

    if (_input_eos && _input_total_rows == 0) {
        *eos = true;
        return Status::OK();
    }

    // 3. 如果当前计算的pos到达partition end,初始化next partition
    //    重置agg function state(todo)
    *next_partition = _init_next_partition(found_partition_end);

    // 4. 初始化窗口结果列
    RETURN_IF_ERROR(_init_result_columns());
    return Status::OK();
}
```

### close
```C++
Status VAnalyticEvalNode::close(RuntimeState* state) {
    if (is_closed()) return Status::OK();

    release_resource(state);
    return ExecNode::close(state);
}

void VAnalyticEvalNode::release_resource(RuntimeState* state) {
    if (is_closed()) {
        return;
    }
    for (auto* agg_function : _agg_functions) {
        agg_function->close(state);
    }

    _destroy_agg_status();
    _release_mem();
    return ExecNode::release_resource(state);
}
```

# VAssertNumRowsNode
用于子链接解关联，例如
```sql
explain graph
select
    -- 子链接，最多返回一行
    (select count(i1) from t2 group by i2)
    from t1;
```
理论上所有的子链接都可以转Join，上面的SQL可以等价转换为`LEFT OUTR JOIN`
```sql
explain graph
select cnt from
	t1 left outer join (
        -- 是子链接，最多返回一行，添加一个assert算子终止查询
        select count(i1) as cnt from t2 group by i2) tmp2
    ON 1=1
```

```C++
Status ExecNode::create_node(RuntimeState* state, ObjectPool* pool,
        const TPlanNode& tnode, const DescriptorTbl& descs, ExecNode** node) {
            ......
    // 生成ExecNode
    case TPlanNodeType::ASSERT_NUM_ROWS_NODE:
        *node = pool->add(new vectorized::VAssertNumRowsNode(
            pool, tnode, descs));
        return Status::OK();
            ....
}
```
```C++
Status PipelineFragmentContext::_build_pipelines(ExecNode* node, PipelinePtr cur_pipe) {
            ......
    // 生成buildOperator
    case TPlanNodeType::ASSERT_NUM_ROWS_NODE: {
        RETURN_IF_ERROR(_build_pipelines(node->child(0), cur_pipe));
        // AssertNumRowsOperatorBuilder是StreamingOperator的一种
        // 通过build_operator生成AssertNumRowsOperator
        OperatorBuilderPtr builder =
                std::make_shared<AssertNumRowsOperatorBuilder>(node->id(), node);
        RETURN_IF_ERROR(cur_pipe->add_operator(builder));
        break;
    }
            .......
}
```

```plantuml
@startuml
class VAssertNumRowsNode {
    - int64_t _desired_num_rows
    - const std::string _subquery_string
    - TAssertion::type _assertion
    + Status pull(RuntimeState* state,\n\tvectorized::Block* output_block, bool* eos)
}

class AssertNumRowsOperatorBuilder {
    + OperatorPtr build_operator()
}

class AssertNumRowsOperator {

}
note right: 是一种StreamingOperator\n其中，StreamingNodeType是VAssertNumRowsNode

class StreamingOperator {
    # StreamingNodeType* _node
    # bool _use_projection
    + Status get_block(RuntimeState* state, vectorized::Block* block,\n\tSourceState& source_state)
}
note bottom : 定义了get_block接口实现从_node的pull接口拉取数据

AssertNumRowsOperator -down-|> StreamingOperator : 继承
VAssertNumRowsNode -right-o AssertNumRowsOperator

AssertNumRowsOperatorBuilder -down-> AssertNumRowsOperator : PipelineFragmentContext::_build_pipeline_tasks生成Operator
@enduml
```
`AssertNumRowsOperatorBuilder`是`StreamingOperator`的一种，通过`VAssertNumRowsNode::pull`来获取`Block`。
```C++
VAssertNumRowsNode::VAssertNumRowsNode(ObjectPool* pool,
        const TPlanNode& tnode, const DescriptorTbl& descs) :
        ExecNode(pool, tnode, descs),
          _desired_num_rows(tnode.assert_num_rows_node.desired_num_rows),
          _subquery_string(tnode.assert_num_rows_node.subquery_string) {
    if (tnode.assert_num_rows_node.__isset.assertion) {
        _assertion = tnode.assert_num_rows_node.assertion;
    } else {
        _assertion = TAssertion::LE;
    }
}

Status VAssertNumRowsNode::pull(doris::RuntimeState* state, vectorized::Block* block, bool* eos) {
    _num_rows_returned += block->rows();
    bool assert_res = false;
    switch (_assertion) {
    case TAssertion::EQ:
        assert_res = _num_rows_returned == _desired_num_rows;
        break;
                ......
    }

    // assert_res为ture,返回Cancel，否则OK
    if (!assert_res) {
            ......
        return Status::Cancelled(......);
    }
    return Status::OK();
}

Status VAssertNumRowsNode::get_next(RuntimeState* state, Block* block, bool* eos) {
    RETURN_IF_ERROR(child(0)->get_next_after_projects(
            state, block, eos,std::bind((Status(ExecNode::*)
            (RuntimeState*, vectorized::Block*, bool*)) &ExecNode::get_next,
                _children[0], std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)));
    return pull(state, block, eos);
}
```

# scan
```plantuml
PInternalServiceImpl -> FragmentMgr:exec_plan_fragment
FragmentMgr -> FragmentMgr:_get_query_ctx
group : _get_query_ctx
FragmentMgr -> QueryContext:create_shared
note left of QueryContext: 创建QueryContext
FragmentMgr -> FragmentMgr:_set_scan_concurrency

FragmentMgr -> QueryContext:set_thread_token
note left of QueryContext: resource_limit.cpu_limit是否设置\nmode=concurrent(并行)
end group

```

```C++
void QueryContext::set_thread_token(int concurrency, bool is_serial) {
    _thread_token = _exec_env->scanner_scheduler()->new_limited_scan_pool_token(
            is_serial ? ThreadPool::ExecutionMode::SERIAL
                        : ThreadPool::ExecutionMode::CONCURRENT,
            concurrency);
}
```

```plantuml
@startuml
class ScannerScheduler {
    - BlockingQueue<ScannerContext*>** _pending_queues
    - std::unique_ptr<PriorityThreadPool> _local_scan_thread_pool
    - std::unique_ptr<ThreadPool> _remote_scan_thread_pool
    - std::unique_ptr<ThreadPool> _limited_scan_thread_pool
}
@enduml
```