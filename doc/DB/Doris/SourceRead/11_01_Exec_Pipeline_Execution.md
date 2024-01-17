
Pipeline调用的入口
```C++
Status FragmentMgr::exec_plan_fragment(const TPipelineFragmentParams& params)
```
Pipeline的执行模块： 负责PipeLine Task和原有的QueryPlan的翻译工作，执行计划的PipeLine化和执行逻辑重构。整体执行流程如下：

```plantuml
@startuml
PInternalServiceImpl -> FragmentMgr:exec_plan_fragment\n(TPipelineFragmentParams)
FragmentMgr -> PipelineFragmentContext:new
FragmentMgr -[#FF9F33]> PipelineFragmentContext:prepare
activate PipelineFragmentContext #FF9F33
PipelineFragmentContext -> ExecNode:create_tree
note left of ExecNode #FF5733 : 1. 创建物理执行树\n含ExecNode的init
PipelineFragmentContext -> ExecNode:prepare
note left of ExecNode #FF5733 : 2. 从根节点(_root_plan)递归地prepare

alt request.fragment.__isset.output_sink
    PipelineFragmentContext -[#4D81F2]> DataSink :create_data_sink
    note left of DataSink #4D81F2 : _sink保存创建的DataSink\n例如,VDataStreamSender
end
PipelineFragmentContext -> PipelineFragmentContext :_build_pipelines
note over of PipelineFragmentContext #DAF7A6 : (1) 自下而上构建Pipeline
alt _sink不为空
    PipelineFragmentContext -[#4D81F2]> PipelineFragmentContext:_create_sink
    note over of PipelineFragmentContext #4D81F2 : 构造DataSink\n_sink是_root_pipeline的sink节点
end
PipelineFragmentContext -> PipelineFragmentContext:_build_pipeline_tasks
note over of PipelineFragmentContext #DAF7A6 : (2) 将Pipelines转化为PipelineTask\n调度模块以PipelineTask为基本调度单位

group _build_pipeline_tasks:
    PipelineFragmentContext -> Pipeline:build_operators
    note left of Pipeline : 一组Operator\n左端是source, 右端是root
    PipelineFragmentContext -> PipelineTask:new
    PipelineFragmentContext -> PipelineTask:prepare
end group

PipelineFragmentContext --> FragmentMgr
deactivate PipelineFragmentContext

FragmentMgr -[#FF9F33]> PipelineFragmentContext:submit
activate PipelineFragmentContext #FF9F33
PipelineFragmentContext -> TaskScheduler:schedule_task
note left of TaskScheduler #DAF7A6 : (3) PipelineTask提交到调度器
TaskScheduler -> PipelineTask:execute
note left of PipelineTask #FF5733 : 3. 含Operator的open, get_block, close
PipelineFragmentContext --> FragmentMgr
deactivate PipelineFragmentContext
@enduml
```

# PipelineFragment prepare

```C++
Status PipelineFragmentContext::prepare(
        const doris::TPipelineFragmentParams& request, const size_t idx) {
    const auto& local_params = request.local_params[idx];
    _runtime_profile.reset(new RuntimeProfile("PipelineContext"));
    auto* fragment_context = this;

    // 1. 创建并初始化RuntimeState
    _runtime_state = RuntimeState::create_unique(
            local_params, request.query_id,
            request.fragment_id, request.query_options,
            _query_ctx->query_globals, _exec_env);
    _runtime_state->set_query_ctx(_query_ctx.get());
    _runtime_state->set_query_mem_tracker(_query_ctx->query_mem_tracker);
    _runtime_state->runtime_filter_mgr()->init();
    _runtime_state->set_be_number(local_params.backend_num);
                ......
    auto* desc_tbl = _query_ctx->desc_tbl;
    _runtime_state->set_desc_tbl(desc_tbl);

    // 2. 创建用于构建Pipeline的ExecNode
    RETURN_IF_ERROR_OR_CATCH_EXCEPTION(ExecNode::create_tree(
            _runtime_state.get(), _runtime_state->obj_pool(),
            request.fragment.plan, *desc_tbl, &_root_plan));

    // 3. 设置exchangenode的sender数量
    //    在VExchangeNode::prepare根据num_senders
    //    初始化PipSenderQueue的_num_remaining_senders
    std::vector<ExecNode*> exch_nodes;
    _root_plan->collect_nodes(TPlanNodeType::EXCHANGE_NODE, &exch_nodes);
    for (ExecNode* exch_node : exch_nodes) {
        int num_senders = find_with_default(request.per_exch_num_senders, exch_node->id(), 0);
        static_cast<vectorized::VExchangeNode*>(exch_node)->set_num_senders(num_senders);
    }

    // 4. 调用Exec Tree所有prepare
    RETURN_IF_ERROR(_root_plan->prepare(_runtime_state.get()));
    
    // 5. 收集ScanNode并设置scan ranges
    std::vector<ExecNode*> scan_nodes;
    std::vector<TScanRangeParams> no_scan_ranges;
    _root_plan->collect_scan_nodes(&scan_nodes);
    for (int i = 0; i < scan_nodes.size(); ++i) {
        ExecNode* node = scan_nodes[i];
        if (typeid(*node) == typeid(vectorized::NewOlapScanNode) ||
            typeid(*node) == typeid(vectorized::NewFileScanNode) ||
            typeid(*node) == typeid(vectorized::NewOdbcScanNode) ||
            typeid(*node) == typeid(vectorized::NewEsScanNode) ||
            typeid(*node) == typeid(vectorized::VMetaScanNode) ||
            typeid(*node) == typeid(vectorized::NewJdbcScanNode)) {
            auto* scan_node = static_cast<vectorized::VScanNode*>(scan_nodes[i]);
            auto scan_ranges = find_with_default(local_params.per_node_scan_ranges,
                    scan_node->id(),no_scan_ranges);
            const bool shared_scan = find_with_default(local_params.per_node_shared_scans, 
                    scan_node->id(), false);
            scan_node->set_scan_ranges(scan_ranges);
            scan_node->set_shared_scan(_runtime_state.get(), shared_scan);
        } else {
            ScanNode* scan_node = static_cast<ScanNode*>(node);
            auto scan_ranges = find_with_default(local_params.per_node_scan_ranges,
                    scan_node->id(), no_scan_ranges);
            scan_node->set_scan_ranges(scan_ranges);
        }
    }

    _runtime_state->set_per_fragment_instance_idx(local_params.sender_id);
    _runtime_state->set_num_per_fragment_instances(request.num_senders);

    // 6. 创建DataSink并执行Sink的init和prepare
    if (request.fragment.__isset.output_sink) {
        RETURN_IF_ERROR_OR_CATCH_EXCEPTION(DataSink::create_data_sink(
                _runtime_state->obj_pool(), request.fragment.output_sink,
                request.fragment.output_exprs, request, idx, _root_plan->row_desc(),
                _runtime_state.get(), &_sink, *desc_tbl));
    }

    // 7. 递归地创建Pipeline树, 此过程在有依赖的地方生成依赖
    //    例如, HASH_JOIN_NODE和CROSS_JOIN_NODE

    // 7.1 根据_root_plan生成_root_pipeline
    _root_pipeline = fragment_context->add_pipeline();
    RETURN_IF_ERROR(_build_pipelines(_root_plan, _root_pipeline));
    if (_sink) {
        // 7.2 生成sink,并为_root_plan设置sink node
        RETURN_IF_ERROR(_create_sink(request.local_params[idx].sender_id,
                request.fragment.output_sink, _runtime_state.get()));
    }

    // 8. 根据上面生成的_pipelines, 生成PipelineTask
    //    并执行PipelineTask::prepare
    RETURN_IF_ERROR(_build_pipeline_tasks(request));

    _prepared = true;
    return Status::OK();
}
```

## 创建ExecNode
`ExecNode::create_tree`递归地自上而下地构建物理执行树，`ExecNode** root`是物理执行树的根节点，并执行`ExecNode::init`。
```C++
Status ExecNode::create_tree(RuntimeState* state, ObjectPool* pool,
        const TPlan& plan, const DescriptorTbl& descs, ExecNode** root) {
    int node_idx = 0;
    // 创建Exec Tree
    RETURN_IF_ERROR(create_tree_helper(state, pool, plan.nodes,
            descs, nullptr, &node_idx, root));
    return Status::OK();
}


Status ExecNode::create_tree_helper(RuntimeState* state, ObjectPool* pool,
        const std::vector<TPlanNode>& tnodes, const DescriptorTbl& descs,
        ExecNode* parent, int* node_idx, ExecNode** root) {
    const TPlanNode& tnode = tnodes[*node_idx];
    int num_children = tnodes[*node_idx].num_children;
    ExecNode* node = nullptr;
    // 1. 根据tnode.node_type创建ExecNode
    RETURN_IF_ERROR(create_node(state, pool, tnodes[*node_idx], descs, &node));

    // 2. 利用创建的ExecNode, 构建ExecNode树
    if (parent != nullptr) {
        parent->_children.push_back(node);
    } else {
        *root = node;
    }

    // 3. 递归地处理孩子
    for (int i = 0; i < num_children; i++) {
        ++*node_idx;
        RETURN_IF_ERROR(create_tree_helper(state, pool,
                tnodes, descs, node, node_idx, nullptr));
    }

    // 4. init Exec
    RETURN_IF_ERROR(node->init(tnode, state));
    return Status::OK();
}


Status ExecNode::create_node(RuntimeState* state, ObjectPool* pool,
        const TPlanNode& tnode, const DescriptorTbl& descs, ExecNode** node) {
    switch (tnode.node_type) {
                ......
    case TPlanNodeType::ANALYTIC_EVAL_NODE:
        *node = pool->add(new vectorized::VAnalyticEvalNode(pool, tnode, descs));
        return Status::OK();
    default: { ...... }

    return Status::OK();
}
```

```plantuml
@startuml
class ExecNode {
    # vectorized::VExprContextSPtrs _conjuncts
    # vectorized::VExprContextSPtrs _projections

    # std::vector<ExecNode*> _children

    + ExecNode* child(int i)

    + virtual Status init(const TPlanNode& tnode, RuntimeState* state)
    + virtual Status prepare(RuntimeState* state)
    + virtual Status open(RuntimeState* state)
    + virtual Status get_next(RuntimeState* state, vectorized::Block* block, bool* eos)
    + virtual Status close(RuntimeState* state)

    + Status get_next_after_projects(RuntimeState* state, vectorized::Block* block,\n\tbool* eos, const std::function<Status(RuntimeState*, vectorized::Block*,\n\tbool*)>& fn, bool clear_data = true)

    + virtual Status sink(doris::RuntimeState* state,vectorized::Block* input_block, bool eos)
    + virtual Status pull(doris::RuntimeState* state, vectorized::Block* output_block, bool* eos)
}
@enduml
```

## 创建DataSink
`DataSink`对外提供了两个静态接口`create_data_sink`将`TDataSink& thrift_sink`生成`DataSink`类。

```C++
class DataSink {
public:
    static Status create_data_sink(ObjectPool* pool, const TDataSink& thrift_sink,
            const std::vector<TExpr>& output_exprs,
            const TPlanFragmentExecParams& params,
            const RowDescriptor& row_desc, RuntimeState* state,
            std::unique_ptr<DataSink>* sink, DescriptorTbl& desc_tbl);

    static Status create_data_sink(ObjectPool* pool, const TDataSink& thrift_sink,
            const std::vector<TExpr>& output_exprs,
            const TPipelineFragmentParams& params,
            const size_t& local_param_idx, const RowDescriptor& row_desc,
            RuntimeState* state, std::unique_ptr<DataSink>* sink,
            DescriptorTbl& desc_tbl);
```
类图
```plantuml
@startuml

class DataSink {
    + virtual Status init(const TDataSink& thrift_sink)
    + virtual Status prepare(RuntimeState* state)
    + virtual Status open(RuntimeState* state) = 0
    + virtual Status send(RuntimeState* state, \n\tvectorized::Block* block, bool eos = false)
    + virtual Status try_close(RuntimeState* state, Status exec_status)
    + virtual bool is_close_done() { return true; }
    + virtual Status close(RuntimeState* state, Status exec_status)
}
note bottom: 派生类需实现send接口，否则报Not support send block

class VDataStreamSender {
    + Status send(RuntimeState* state, \n\tBlock* block, bool eos = false)
}

VOdbcTableSink -down-|> VTableSink
VJdbcTableSink -down-|> VTableSink
VTableSink -left-|> DataSink

MemoryScratchSink -down-|> DataSink
VResultSink -down-|> DataSink : 继承
VResultFileSink -down-|> DataSink : 继承
VDataStreamSender -down-|> DataSink
@enduml
```

## 生成Pipeline树
通过`PipelineFragmentContext::_build_pipelines`来构建Pipeline树，主要根据`ExecNode::type()`创建`OperatorBuilder`,`OperatorBuilder`提供了`OperatorPtr build_operator()`来生成Operator，首先了解Pipeline的数据结构

```plantuml
@startuml
class Pipeline {
    - std::vector<OperatorPtr> _operators
    - OperatorBuilders _operator_builders
    - std::vector<std::pair<int, std::weak_ptr<Pipeline>>> _parents
    - std::vector<std::pair<int, std::shared_ptr<Pipeline>>> _dependencies

    - OperatorBuilderPtr _sink

    + Status add_operator(OperatorBuilderPtr& op)
    + Status build_operators()

    + void add_dependency(std::shared_ptr<Pipeline>& pipeline)
    + void finish_one_dependency(int dep_opr, int dependency_core_id)
    + bool has_dependency()

    + Status set_sink(OperatorBuilderPtr& sink_operator)
    + OperatorBuilderBase* sink()
}
note top : Pipeline持有依赖的Pipeline, PipelineTask完成调用finish_one_dependency删除依赖

class OperatorBuilderBase {
    - const int32_t _id
    - const std::string _name

    + OperatorPtr build_operator() = 0
    + const RowDescriptor& row_desc() = 0

    + virtual bool is_sink() const
    + virtual bool is_source() const
}

class OperatorBuilder {
    - NodeType* _node // ExecNode
    + virtual OperatorPtr build_operator() = 0
    + const RowDescriptor& row_desc()
    + NodeType* exec_node()
}

interface ExecNode {
    + const RowDescriptor& row_desc() const

    + Status get_next_after_projects(RuntimeState* state,\n\tvectorized::Block* block, bool* eos,\n\tconst std::function<Status(RuntimeState*, \n\tvectorized::Block*, bool*)>& fn, bool clear_data = true)
}

OperatorBuilder -up-o Pipeline : add_operator添加元素到_operator_builders\nbuild_operators接口生成Operator\n并添加到_operators
OperatorBase -left-> OperatorBuilder : build_operators()\n生成Operator

OperatorBuilder -down-|> OperatorBuilderBase
ExecNode -up-o OperatorBuilder
@enduml
```


`PipelineFragmentContext::_build_pipelines`实现Pipeline的拆解以及`HASH_JOIN_NODE`存在依赖的算子的Pipeline的生成。
```C++
Status PipelineFragmentContext::_build_pipelines(ExecNode* node, PipelinePtr cur_pipe) {
    auto node_type = node->type();
    switch (node_type) {
        ......
    case TPlanNodeType::SELECT_NODE: {
        RETURN_IF_ERROR(_build_pipelines(node->child(0), cur_pipe));
        OperatorBuilderPtr builder = std::make_shared<SelectOperatorBuilder>(node->id(), node);
        RETURN_IF_ERROR(cur_pipe->add_operator(builder));
        break;
    }
                ......
}
```

### 阻塞算子的Pipeline拆解
`PipelineFragmentContext::_build_pipelines`实现Pipeline的拆解。窗口，Sort，Join Build，Agg，Scan，Exchange会将他们拆解成Sink和Source0，达到阻塞逻辑和Pipeline隔离。例如Hash Join
```C++
// 存在依赖的算子
case TPlanNodeType::HASH_JOIN_NODE: {
    auto* join_node = assert_cast<vectorized::HashJoinNode*>(node);
    // 1. 生成Build端的Pipeline(new_pip)
    // 1.1 递归地为build端生成Pipeline
    auto new_pipe = add_pipeline();
    if (join_node->should_build_hash_table()) {
        RETURN_IF_ERROR(_build_pipelines(node->child(1), new_pipe));
    } else {
        OperatorBuilderPtr builder = std::make_shared<EmptySourceOperatorBuilder>(
                node->child(1)->id(), node->child(1)->row_desc(), node->child(1));
        new_pipe->add_operator(builder);
    }
    // 1.2 为build端pipeline生成sink算子
    OperatorBuilderPtr join_sink =
            std::make_shared<HashJoinBuildSinkBuilder>(node->id(), join_node);
    RETURN_IF_ERROR(new_pipe->set_sink(join_sink));


    // 2. 生成Prob端的Pipeline
    // 2.1 递归地为Prob端生成Pipeline(cur_pie)
    RETURN_IF_ERROR(_build_pipelines(node->child(0), cur_pipe));
    // 2.2 生成HashJoinProbeOperatorBuilder并添加到cur_pipe中
    OperatorBuilderPtr join_source =
            std::make_shared<HashJoinProbeOperatorBuilder>(node->id(), join_node);
    RETURN_IF_ERROR(cur_pipe->add_operator(join_source));

    // 3. 生成依赖(Build构建完成,再执行Prob)
    cur_pipe->add_dependency(new_pipe);
    break;
}
```
例如，窗口
```C++
case TPlanNodeType::ANALYTIC_EVAL_NODE: {
    // 1. 创建Pipeline
    auto new_pipeline = add_pipeline();
    // 2. 递归地对其子节点构建Pipeline
    RETURN_IF_ERROR(_build_pipelines(node->child(0), new_pipeline));

    OperatorBuilderPtr analytic_sink =
            std::make_shared<AnalyticSinkOperatorBuilder>(node->id(), node);
    RETURN_IF_ERROR(new_pipeline->set_sink(analytic_sink));

    OperatorBuilderPtr analytic_source =
            std::make_shared<AnalyticSourceOperatorBuilder>(node->id(), node);
    RETURN_IF_ERROR(cur_pipe->add_operator(analytic_source));
    break;
}
```

### 集合运算Pipeline生成
intersect，except和UNION

UNION Pipeline生成
```C++
// UNION上面会生成一个AGG进行去重
case TPlanNodeType::UNION_NODE: {
    auto* union_node = assert_cast<vectorized::VUnionNode*>(node);
    if (union_node->children_count() == 0 &&
        union_node->get_first_materialized_child_idx() == 0) { // only have const expr
        OperatorBuilderPtr builder =
                std::make_shared<ConstValueOperatorBuilder>(node->id(), node);
        RETURN_IF_ERROR(cur_pipe->add_operator(builder));
    } else {
        int child_count = union_node->children_count();
        auto data_queue = std::make_shared<DataQueue>(child_count);
        for (int child_id = 0; child_id < child_count; ++child_id) {
            // 每个孩子生成一条Pipeline
            auto new_child_pipeline = add_pipeline();
            RETURN_IF_ERROR(_build_pipelines(union_node->child(child_id), new_child_pipeline));
            OperatorBuilderPtr child_sink_builder = std::make_shared<UnionSinkOperatorBuilder>(
                    union_node->id(), child_id, union_node, data_queue);
            RETURN_IF_ERROR(new_child_pipeline->set_sink(child_sink_builder));
        }
        OperatorBuilderPtr source_builder = std::make_shared<UnionSourceOperatorBuilder>(
                node->id(), union_node, data_queue);
        RETURN_IF_ERROR(cur_pipe->add_operator(source_builder));
    }
    break;
}
```
```C++
// 集合运算
// open阶段hash_table_build拉取数据
case TPlanNodeType::INTERSECT_NODE: {
    RETURN_IF_ERROR(_build_operators_for_set_operation_node<true>(node, cur_pipe));
    break;
}
case TPlanNodeType::EXCEPT_NODE: {
    RETURN_IF_ERROR(_build_operators_for_set_operation_node<false>(node, cur_pipe));
    break;
}

template <bool is_intersect>
Status PipelineFragmentContext::_build_operators_for_set_operation_node(ExecNode* node,
                                                                        PipelinePtr cur_pipe) {
    auto build_pipeline = add_pipeline();
    // 1. 0号孩子是一条Pipeline(build_pipeline)
    RETURN_IF_ERROR(_build_pipelines(node->child(0), build_pipeline));
    OperatorBuilderPtr sink_builder =
            std::make_shared<SetSinkOperatorBuilder<is_intersect>>(node->id(), node);
    RETURN_IF_ERROR(build_pipeline->set_sink(sink_builder));

    for (int child_id = 1; child_id < node->children_count(); ++child_id) {
        // 2. 1~N孩子是一条Pipeline(probe_sink_builder)
        auto probe_pipeline = add_pipeline();
        RETURN_IF_ERROR(_build_pipelines(node->child(child_id), probe_pipeline));
        OperatorBuilderPtr probe_sink_builder =
                std::make_shared<SetProbeSinkOperatorBuilder<is_intersect>>(node->id(), child_id,
                                                                            node);
        RETURN_IF_ERROR(probe_pipeline->set_sink(probe_sink_builder));
    }

    OperatorBuilderPtr source_builder =
            std::make_shared<SetSourceOperatorBuilder<is_intersect>>(node->id(), node);
    return cur_pipe->add_operator(source_builder);
}
```

## 生成Operator

对于`build_operator`的实现依赖宏定义进行代码生成
```C++
#define OPERATOR_CODE_GENERATOR(NAME, SUBCLASS)                       \
    NAME##Builder::NAME##Builder(int32_t id, ExecNode* exec_node)     \
            : OperatorBuilder(id, #NAME, exec_node) {}                \
                                                                      \
    OperatorPtr NAME##Builder::build_operator() {                     \
        return std::make_shared<NAME>(this, _node);                   \
    }                                                                 \
                                                                      \
    NAME::NAME(OperatorBuilderBase* operator_builder, ExecNode* node) \
            : SUBCLASS(operator_builder, node) {};
```
例如, `ANALYTIC_EVAL_NODE`它对应的Pipeline的Sink是`AnalyticSinkOperatorBuilder`,Operator是`AnalyticSourceOperatorBuilder`
```C++
// Sink生成StreamingOperator，基类是StreamingOperator
// build_operator生成AnalyticSinkOperator，其ExecNode是VAnalyticEvalNode
OPERATOR_CODE_GENERATOR(AnalyticSinkOperator, StreamingOperator)

// Source生成SourceOperator，基类是SourceOperator
// build_operator生成AnalyticSourceOperator，其ExecNode是VAnalyticEvalNode
OPERATOR_CODE_GENERATOR(AnalyticSourceOperator, SourceOperator)
```

### 普通算子

```plantuml
@startuml
class OperatorBuilderBase {
    + virtual bool is_sink() const
    + virtual bool is_source() const
    + virtual bool can_read()
    + virtual bool can_write()

    + virtual OperatorPtr build_operator() = 0
    + virtual const RowDescriptor& row_desc() = 0
    + int32_t id()
}

class OperatorBuilder {
    # NodeType* _node
    + const RowDescriptor& row_desc()
    + NodeType* exec_node() const // _node
}
note left : NodeType派生自ExecNode

class DataSinkOperatorBuilder {
    # SinkType* _sink
}
note right : SinkType派生自DataSink


class StatefulOperator {
    + Status get_block(RuntimeState* state, \n\tvectorized::Block* block,\n\tSourceState& source_state)
}
note bottom of StatefulOperator: 自定义实现了get_block\n一行可以匹配产生多行输出的计算\n算子内部实现(!need_more_input_data())\n例如，hash join probe operator
class StreamingOperator {
    # NodeType* _node
    # bool _use_projection
    + Status get_block(RuntimeState* state, vectorized::Block*\n\tblock, SourceState& source_state)
    + Status sink(RuntimeState* state, vectorized::Block*\n\tin_block,SourceState source_state)
}

class SourceOperator {
    + bool can_read()
    + Status get_block(RuntimeState* state,\n\tvectorized::Block*block,\n\tSourceState& source_state)
}
note bottom : 自定义实现了get_block,调用自身pull接口

class DataSinkOperator {
    + bool can_write()
    + Status sink(RuntimeState* state, vectorized::Block* \n\tin_block,SourceState source_state)
    + Status close(RuntimeState* state)
    + Status finalize(RuntimeState* state)
}
note bottom : 和DataSink一一对应\n并非所有的SinkOperators都继承自它\nAnalyticSinkOperator是一种StreamingOperator

interface OperatorBase {
    - OperatorBuilderBase* _operator_builder
    - OperatorPtr _child
    + virtual bool is_pending_finish()
    + Status open(RuntimeState* state) = 0
    + Status get_block(RuntimeState* state, vectorized::Block*\n\tblock, SourceState& source_state)
}
note left : get_block是执行\nPipelineTask的主要接口

class EmptySourceOperator {

}
note bottom: HashJoin(!should_build_hash_table)Build测\nPipeline的Source Operator\nget_block直接返回OK，result_state置为\nSourceState::FINISHED

StatefulOperator -up-|> StreamingOperator
SourceOperator -up-|> StreamingOperator
StreamingOperator -up-|> OperatorBase
DataSinkOperator -up-|> OperatorBase
EmptySourceOperator -up-|> OperatorBase

OperatorBuilderBase -down-> OperatorBase
OperatorBuilder -down-|> OperatorBuilderBase : 继承
DataSinkOperatorBuilder -down-|> OperatorBuilderBase
@enduml
```

### 集合运算

```plantuml
@startuml
class UnionSourceOperator {
}
note bottom : 借助DataQueue自定义实现了get_block\nN个孩子都Finish,该算子才FINISHED

class UnionSinkOperator {
    - int _cur_child_id;
    - std::shared_ptr<DataQueue> _data_queue
}
note bottom: 自定义sink接口，向DataQueue中push_block

class SetSourceOperator {
}

class SetProbeSinkOperator {
}
note top : 自定义sink,调用VSetOperationNode::sink_probe

UnionSourceOperator -up-|> SourceOperator
SetSourceOperator -down-|> SourceOperator : <VSetOperationNode>
SetSinkOperator -down-|> StreamingOperator : <VSetOperationNode>

UnionSinkOperator -up-|> StreamingOperator

SetProbeSinkOperator -down-|> StreamingOperator : <VSetOperationNode>
SourceOperator -left-|> StreamingOperator
@enduml
```

## 生成PipelineTask
`PipelineFragmentContext::_build_pipeline_tasks`接口将`Pipeline`生成`PipelineTask`。

```C++
Status PipelineFragmentContext::_build_pipeline_tasks(
        const doris::TPipelineFragmentParams& request) {
    _total_tasks = 0;
    // 1. 生成PipelineTask
    for (PipelinePtr& pipeline : _pipelines) {
        // 1.1 生成Sink Operator
        auto sink = pipeline->sink()->build_operator();
        sink->init(request.fragment.output_sink);

        // 1.2 将Pipeline中的_operator_builders转换成Operators
        RETURN_IF_ERROR(pipeline->build_operators());

        // 1.3 生成PipelineTask,其中_operators=pipeline->_operators
        //     _source(_operators.front()), _root(_operators.back())
        auto task = std::make_unique<PipelineTask>(pipeline, _total_tasks++,
                _runtime_state.get(), sink, this, pipeline->pipeline_profile());
        sink->set_child(task->get_root());
        _tasks.emplace_back(std::move(task));
    }

    // 2. PipelineTask prepare
    for (auto& task : _tasks) {
        RETURN_IF_ERROR(task->prepare(_runtime_state.get()));
    }

    return Status::OK();
}
```
在`PipelineTask::execute`中调用Operator的`open`, `get_block`接口

```plantuml
@startuml
class PipelineTask {
    - PipelinePtr _pipeline
    - Operators _operators // 取pipeline->_operators
    + bool has_dependency()

    + Status prepare(RuntimeState* state)
    + Status execute(bool* eos);
}

interface OperatorBase {
    + virtual Status init(const TDataSink& tsink)
    + virtual Status prepare(RuntimeState* state) = 0;
    + virtual Status open(RuntimeState* state) = 0;
    + virtual Status get_block(RuntimeState* runtime_state,\n\tvectorized::Block* block, SourceState& result_state)
    + virtual Status close(RuntimeState* state)
}

OperatorBase -up-o PipelineTask
@enduml
```

```C++
Status PipelineTask::execute(bool* eos) {
    *eos = false;
    if (!_opened) {
        // for o : _operators o->open()
        // _sink->open _opened = true
        auto st = _open();
        if (....) {
            set_state(....);
            return Status::OK();
        }
    }

    while (!_fragment_context->is_canceled()) {
        _block->clear_column_data(_root->row_desc().num_materialized_slots());
        auto* block = _block.get();

        // Pull block from operator chain
        _get_block_counter->update(1);
        RETURN_IF_ERROR(_root->get_block(_state, block, _data_state));

        *eos = _data_state == SourceState::FINISHED;
        if (_block->rows() != 0 || *eos) {
            auto status = _sink->sink(_state, block, _data_state);
            *eos = status.is<ErrorCode::END_OF_FILE>() ? true : *eos;
            if (*eos) { // just return, the scheduler will do finish work
                break;
            }
        }
    }

    return Status::OK();
}
```

# submit tasks

```plantuml
@startuml
class PipelineFragmentContext {
 - ExecEnv* _exec_env
}

class ExecEnv {
    - pipeline::TaskScheduler* _pipeline_task_scheduler
    - pipeline::TaskScheduler* _pipeline_task_group_scheduler 
}

class TaskScheduler {
 + Status start();
 + void shutdown()
 + Status schedule_task(PipelineTask* task)
}

ExecEnv -up-o PipelineFragmentContext
TaskScheduler -left-o ExecEnv
@enduml
```

忽略异常,处理了解主流程
```C++
Status PipelineFragmentContext::submit() {
            ......
    _submitted = true;

    Status st;
    int submit_tasks = 0;
    // 1. 从_exec_env中读取TaskScheduler
    auto* scheduler = _exec_env->pipeline_task_scheduler();
    if (get_task_group()) {
        scheduler = _exec_env->pipeline_task_group_scheduler();
    }

    // 2. 提交PipelineTasks到TaskScheduler
    //    将任务放在_task_queue通过后台worker线程调度
    for (auto& task : _tasks) {
        st = scheduler->schedule_task(task.get());
        submit_tasks++;
    }
            ......
    return st;
}
```



