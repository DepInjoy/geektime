查询优化的入口在`src\backend\optimizer\plan\planner.c`的`planner`它支持通过`planner_hook`来自定义优化方法，如果没有采用PG自定义的标准的查询优化函数`standard_planner`。

```C++
PlannedStmt * planner(Query *parse, const char *query_string,
                      int cursorOptions, ParamListInfo boundParams)
```

