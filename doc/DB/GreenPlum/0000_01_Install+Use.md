# 源码编译

```shell
./configure --with-perl --with-python --with-libxml --with-gssapi \
	--prefix=/home/xieyue/project/local/gpdb \
	--enable-debug --enable-cassert
# 关闭编译器优化，防止调试时变量被优化而无法查看
sed -i 's/-O3/-O0/g' src/Makefile.global
make -j8 && make -j8 install
```

# 运行

```shell
# 添加环境变量
source /home/xieyue/project/local/gpdb/greenplum_path.sh

make create-demo-cluster
souce gpAux/gpdemo/gpdemo-env.sh
```

Greenplum需要配置ssh免密码登录，单机版只需要配置本机无密码登录即可。

```shell
cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys
```

之后，输入 ssh localhost 进行测试, 如果无需密码可以登录，则配置成功。输入logout退出登录。

GP默认的监听7000端口，`vim $MASTER_DATA_DIRECTORY/postgresql.conf`，将port修改为相应的监听端口，例如`port=54321 `。

遇到`ssh: connect to host localhost port 22: Connection refused`的报错

```shell
# 查看ssh服务状态，如果显示sshd is not running
sudo service ssh status

# 重启ssh服务
sudo service ssh start
```



启动成功之后，在shell，输入`psql postgres`建立连接，在`psql`终端创建数据库、用户并授权。

```sql
-- 创建数据库
CREATE DATABASE db_test;
-- 创建用户并设置密码
CREATE USER user_test WITH PASSWORD 'a';
-- 授权
GRANT ALL PRIVILEGES ON DATABASE db_test TO user_test;
```

通过`Navicat Premium`访问数据库需要，开启用户远端访问，打开`$MASTER_DATA_DIRECTORY/pg_hba.conf`，在尾部追加如下信息：

```shell
host		all		user_test		all		trust
```

执行`gpstop -u`重新加载配置文件。

# 管理工具

`gpconfig`在Greenplum数据库系统中所有的Segment上设置服务器配置参数。其中使用以下环境变量连接到Greenplum数据库的 Master实例并获取系统配置信息：`PGHOST`, `PGPORT`, `PGUSER`, `PGPASSWORD`, `PGDATABASE`。

假如你将连接的端口号修改为了`port=54321 `，那么在你尝试连接使用`gpconfig`的阶段需要执行`export PGPORT=54321`来修改系统配置参数。

## 参考资料

- [Greenplum Doc: 管理工具参考](http://docs-cn.greenplum.org/v6/utility_guide/admin_utilities/util_ref.html)

# 查询优化

配置GUC参数使得在终端可以显示Memo结构

```sql
-- These GUCS print the memo structure to the console and provide the necessary
-- information to follow the logic to derive the optimal plan
SET client_min_messages=log;
SET optimizer_print_memo_after_optimization=on;
SET optimizer_print_optimization_context=on;

-- see the memo at different stages of the optimization process
SET optimizer_print_memo_after_exploration=on;
SET optimizer_print_memo_after_implementation=on;
```

## 参考资料

- [Understanding Orca Memos](https://github.com/greenplum-db/gpdb/blob/master/src/backend/gporca/README.memo.md)