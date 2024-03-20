# 编译
```shell
docker run -it --network=host --name doris-2.0 -v D:/project/docker/doris/.m2:/root/.m2 -v D:/project/docker/doris/doris-2.0/:/root/doris-2.0/ apache/doris:build-env-for-2.0

# 下载代码
git clone https://github.com/apache/doris.git -b branch-2.0
```

2.1分支编译
```shell
# 拉取最新镜像
docker pull apache/doris:build-env-ldb-toolchain-latest

# 
docker run -it --network=host --name doris-2.1 -v D:/project/build/doris/.m2:/root/.m2 -v D:/project/build/doris/.npm:/root/.npm -v D:/project/build/doris/doris-2.1/:/root/doris-2.1/ apache/doris:build-env-ldb-toolchain-latest


```

# 部署
## fe部署
```shell
# 1 修改端口配置, 启动即可
# 1.1 配置文件
vim conf/fe.conf

# 1.2 起停命令
./bin/start_fe.sh --daemon
./bin/stop_fe.sh --daemon

# 1.3 网页连接后台
<fe-ip>:<http_port>
# 默认用户为root, 无密码

# 1.4 mysql连接后台，创建db和user
mysql -h <fe-ip> -P <query_port> -uroot
# 默认用户为root, 无密码
create user 'user_test' identified by '123456a'
create database db_test;
grant all on db_test to user_test;
# 之后便可以用db_test和user_test进行连接
```

## be部署
```shell
# 2. 修改JAVA_HOME及端口配置, 启动, 之后需要将be添加到fe集群
# 2.1 配置文件
vim conf/be.conf

# 2.2 起停命令
./bin/start_be.sh --daemon
./bin/stop_be.sh --daemon

# 2.3 mysql连接后台, 添加be到集群
alter system add backend "<be-ip>:<heartbeat_service_port>"; 

# 2.4 查看be添加状态. 若查询结果显示Alive = true正证明添加成功
show backends;
```
## 查看执行计划

1. `EXPLAIN GRAPH select ...;`或`DESC GRAPH select ...;`：获取执行计划的图形表示，帮助我们可视化查询执行的流程，包括关联路径和数据访问方法。
    ```sql
    -- 获取执行计划的图形表示
    explain graph
    select avg(length(i2)) from t1 group by i1;

    DESC GRAPH
    select avg(length(i2)) from t1 group by i1;
    ```

## 集群管理
`ALTER SYSTEM DECOMMISSION BACKEND`用于节点安全下线

```sql
ALTER SYSTEM DECOMMISSION BACKEND
    "host:heartbeat_port"[,"host:heartbeat_port"...];
```

## 查询分析

```sql
# 打开FE的Report
set enable_profile=true; 
```
之后执行对应的SQL语句之后，在FE的Web页面就可以看到对应SQL语句执行的Report信息

# SQL查询

## 账户管理

设置用户的属性，包括分配给用户的资源、导入cluster等
```sql
SET PROPERTY [FOR 'user'] 'key' = 'value' [, 'key' = 'value']
```


## 资源管理

## catalog
`catalogs()`表函数，生成catalogs临时表，可以查看当前doris中的创建的catalogs信息，该函数用于from子句中。
```sql
desc function catalogs();
select * from catalogs();
```

### 属性设置

设置指定数据目录的属性，仅管理员使用。
```sql
-- 重命名数据目录
ALTER CATALOG catalog_name RENAME new_catalog_name;

-- 设置数据库目录属性
ALTER CATALOG catalog_name SET PROPERTIES (
    'key1' = 'value1' [, 'key' = 'value2']
); 
```

# SHOW

```sql
-- 显示已存在的数据目录(catalog)
SHOW CATALOGS [LIKE]

-- 查看当前DB下的表
-- LIKE可按照表名进行模糊查询，可选，不指定是查看DB下所有表
SHOW [FULL] TABLES [LIKE]
SHOW TABLES FROM <catalog_name>.<db_name>

-- 查看指定表的列信息，也适用于外部数据源
SHOW [FULL] COLUMNS FROM tbl;
```

# 客户端连接
## Navicat显示乱码
> 由于系统本身编码(命令行chcp)中为936(GBK)而不是65001(UTF8)造成
> 执行 chcp查看系统本本身的编码，如果执行结果是Active page code 936
> 修改系统默认的编码格式为 Beta版：使用Unicode UTF-8提供全球语言支持(具体操作步骤参见下方链接)
> 
> 参考链接：[Navicat for Mysql 字段注释中文乱码解决方法](https://blog.csdn.net/qq_39715000/article/details/121425533)

