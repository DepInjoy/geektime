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
        // get_next会调用execute和insert_result
        vectorized_get_next get_next;
        vectorized_get_result insert_result;
        vectorized_closer close;
    };
    executor _executor;
private:
    enum AnalyticFnScope { PARTITION, RANGE, ROWS 
   
    // 从孩子那里获取到的block,在sink接口中向后追加
    std::vector<Block> _input_blocks;
    std::vector<int64_t> input_block_first_row_positions;
                          
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

    std::vector<MutableColumnPtr> _result_window_columns;

    BlockRowPos _order_by_start;
    BlockRowPos _order_by_end;
    BlockRowPos _partition_by_start;
    BlockRowPos _partition_by_end;
    // 在sink中更新
    BlockRowPos _all_block_end;

    // input是否是结束标志
    bool _input_eos = false;
    bool _next_partition = false;
    std::atomic_bool _need_more_input = true;
    BlockRowPos _found_partition_end;
    
    // 读取的数据的总行数
    int64_t _input_total_rows = 0;
    // 输出block索引，在_output_current_block中更新
    int64_t _output_block_index = 0;
    int64_t _window_end_position = 0;
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
    TupleId _intermediate_tuple_id;
    TupleId _output_tuple_id;
	// 前端发送过来计划配置信息
    TAnalyticWindow _window;
	// Frame类型
    AnalyticFnScope _fn_scope;
    TupleDescriptor* _intermediate_tuple_desc;
    TupleDescriptor* _output_tuple_desc;
    std::vector<int64_t> _origin_cols;

    RuntimeProfile::Counter* _evaluation_timer;
    RuntimeProfile::Counter* _memory_usage_counter;
    RuntimeProfile::HighWaterMarkCounter* _blocks_memory_usage;

    std::vector<bool> _change_to_nullable_flags;
};
} // namespace doris::vectorized
```