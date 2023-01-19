# SELECT语句

PG用`SelectStmt`结构来表示其语法解析树。

```c

```



在`src\backend\parser\gram.y`中来表示语法解析

```
simple_select:
			SELECT opt_all_clause opt_target_list
			into_clause from_clause where_clause
			group_clause having_clause window_clause
```

