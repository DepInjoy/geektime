使能`optimizer_trace`,便于跟进计划的优化信息

```sql
set optimizer_trace="enabled=on", end_markers_in_JSON=on;
select * from information_schema.optimizer_trace;
```

查看Mysql的计划
```sql
-- 获取tree格式的执行计划
explain format=tree
SELECT id,set_1 FROM t1;
```