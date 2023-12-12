# 编译
```shell

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
