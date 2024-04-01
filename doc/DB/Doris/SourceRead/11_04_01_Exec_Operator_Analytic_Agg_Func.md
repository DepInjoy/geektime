
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