# 使用

## 建立连接
```shell
psql -h <ip> -p <query port> -d <database> -U <user name>

# 连接PG数据库执行指定SQL文件
psql -h <ip> -p <query port> -d <database> -U <user name> -f xx.sql
```

## 常用SQL
```sql
-- 切换数据库
\c database_name;

-- 退出psql
\q
```

```sql
-- namespace oid是隐藏字段，须显式加oid
SELECT oid, * FROM pg_namespace;

-- 
SELECT * FROM current_schema();
```