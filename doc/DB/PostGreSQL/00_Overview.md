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
