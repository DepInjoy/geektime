

`pg_rules`提供对查询重写规则的信息访问，排除了视图和物化视图的`ON SELECT`规则。

|    列名称    | 描述                                       |
| :----------: | :----------------------------------------- |
| `schemaname` | 包含表的模式名(`pg_namespace.nspname`)     |
| `tablename`  | 规则适用的表名(`pg_class.tablename`)       |
|  `rulename`  | 规则名称(`pg_rewrite.relname`)             |
| `definition` | 规则定义(`pg_get_ruledef(pg_rewrite.oid)`) |

`pg_views`

`pg_matviews`



