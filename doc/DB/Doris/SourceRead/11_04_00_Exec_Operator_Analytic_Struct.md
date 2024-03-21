记录了一些窗口定义中成员，辅助理解代码
```C++
namespace doris::vectorized {
struct BlockRowPos {
    BlockRowPos() : block_num(0), row_num(0), pos(0) {}
    int64_t block_num; //the pos at which block
    int64_t row_num;   //the pos at which row
    int64_t pos;       //pos = all blocks size + row_num
};

class VAnalyticEvalNode : public ExecNode {
                ......
private:
                ......
    struct executor {
        // 这里涉及的类型别名定义看源码
        vectorized_execute execute;
        // 在构造函数根据Frame信息判断并赋值
        // get_next会调用execute和insert_result
        vectorized_get_next get_next;
        vectorized_get_result insert_result;
        vectorized_closer close;
    };
    executor _executor;
private:
    enum AnalyticFnScope { PARTITION, RANGE, ROWS };

    // frame信息，type参数表示ROWS|RANGE
    TAnalyticWindow _window;
    // 根据Frame将其分为PARTITION, RANGE, ROWS三大类
    // 在构造函数判断并赋值，并给_executor.get_next设置执行函数
    AnalyticFnScope _fn_scope;

    // Function计算函数(可能有多个)
    std::vector<AggFnEvaluator*> _agg_functions;
    // Function的表达式，二维数组，[agg function index][expr index]
    std::vector<VExprContextSPtrs> _agg_expr_ctxs;
    // Function表达式列,二维数组[agg function index][expr col index]
    std::vector<std::vector<MutableColumnPtr>> _agg_intput_columns;

    // 前端计划的partition和order表达式
    VExprContextSPtrs _partition_by_eq_expr_ctxs;
    VExprContextSPtrs _order_by_eq_expr_ctxs;


    // partition和order表达式在block的column index,在sink中更新
    std::vector<int64_t> _ordey_by_column_idxs;
    std::vector<int64_t> _partition_by_column_idxs;

    // 窗口函数执行结果,在_init_result_columns根据函数返回类型创建空列
    // _insert_result_info中向其插入数据
    // _execute_for_win_fun函数计算
    std::vector<MutableColumnPtr> _result_window_columns;

    // 用于RANGE处理，二分法计算分组信息
    // 参见_update_order_by_range
    BlockRowPos _order_by_start;
    BlockRowPos _order_by_end;

    // 
    // 参见VAnalyticEvalNode::_init_next_partition
    BlockRowPos _partition_by_start;
    BlockRowPos _partition_by_end;


    // 已获取最后一个block位置信息
    //   block_num : 在_input_blocks的index
    //   row_num   : 当前最后一个block的总行数
    //   pos       : 当前最后一个block开始行位置信息
    // 在VAnalyticEvalNode::sink中更新
    BlockRowPos _all_block_end;

    // 从孩子中获取到的block,在sink接口中向后追加
    std::vector<Block> _input_blocks;
    // 获取到的每个block开始行位置信息
    std::vector<int64_t> input_block_first_row_positions;

    // 读取的数据的总行数，每读取一个Block,自增block_size
    // VAnalyticEvalNode::sink中更新
    int64_t _input_total_rows = 0;

    // input是否是结束标志
    bool _input_eos = false;
    // 记录原始column信息，
    std::vector<int64_t> _origin_cols;


    bool _next_partition = false;
    // 根据目前的partition信息计算是否需要获取更多数据
    // 由VAnalyticEvalNode::whether_need_next_partition计算
    std::atomic_bool _need_more_input = true;
    // 已获取的Block的partition end信息
    // 由VAnalyticEvalNode::_get_partition_by_end计算
    BlockRowPos _found_partition_end;
    

    // 输出block索引，在_output_current_block中更新
    int64_t _output_block_index = 0;


    // 当前output block, 正处理Partition, 函数计算结束的行位置
    // 参见VAnalyticEvalNode::_insert_result_info
    int64_t _window_end_position = 0;
    // 当前output block, 正处理Partition, 行位置
    // 参见VAnalyticEvalNode::_insert_result_info
    int64_t _current_row_position = 0;


    int64_t _rows_start_offset = 0;
    int64_t _rows_end_offset = 0;
    size_t _agg_functions_size = 0;
    bool _agg_functions_created = false;

    /// The offset of the n-th functions.
    std::vector<size_t> _offsets_of_aggregate_states;
    /// The total size of the row from the functions.
    size_t _total_size_of_aggregate_states = 0;
    /// The max align size for functions
    size_t _align_aggregate_states = 1;
    std::unique_ptr<Arena> _agg_arena_pool;
    AggregateDataPtr _fn_place_ptr;

    TTupleId _buffered_tuple_id = 0;

    // 根据计划给窗口算子配置的output_tuple_id和intermediate_tuple_id
    // 从关系的DescriptorTbl& desc_tbl()获取
    // intermediate_slot_desc和output_slot_desc从而对_agg_function的prepare
    TupleId _intermediate_tuple_id;
    TupleId _output_tuple_id;

    // 所在关系输出结果可能含NULL
    // 且函数执行结果不支持null(_agg_functions[i]->data_type()->is_nullable()为false)
    // 在_output_current_block接口会使用
    // 利用make_nullable接口生成DataTypeNullable
    std::vector<bool> _change_to_nullable_flags;


    TupleDescriptor* _intermediate_tuple_desc;
    TupleDescriptor* _output_tuple_desc;


    RuntimeProfile::Counter* _evaluation_timer;
    RuntimeProfile::Counter* _memory_usage_counter;
    
    // block使用的内存，累加block->allocated_bytes()
    // 参见VAnalyticEvalNode::sink
    // block完成输出，自减block->allocated_bytes
    // 参见VAnalyticEvalNode::_output_current_block
    RuntimeProfile::HighWaterMarkCounter* _blocks_memory_usage;
};
} // namespace doris::vectorized
```