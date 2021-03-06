# 编译安装
可以跟着[源码编译安装Postgresql 11.4](https://segmentfault.com/a/1190000019928714)做。

```SHELL
mkdir build
cd build
./configure --prefix=$HOME/project/local/postgres --exec-prefix=$HOME/project/local/postgres
./configure --prefix=/home/local/postgres --exec-prefix=/home/local/postgres
make && make install

cd ../contrib
make && make install
```
```shell
sudo groupadd xy_pg postgres
sudo useradd -g postgres postgres
chown -R postgres:postgres $HOME/project/local/postgres/data
chown -R xy_pg:xy_pg $HOME/project/local/postgres/data
```
后台启动
```shell
nohup pg_ctl -D data/ -l logfile start
```
设置监听整个网络,在`PGData/postgresql.conf`中修改配置
```shell
listen_addresses = '*'

# 修改监听端口
port = 9866
```

配置数据库允许连接，在`PGData/pg_hba.conf`添加
```
# 开启允许远程连接
host    all             all             0.0.0.0/0               md5
```

[PG官方的INSTALL文档](https://www.postgresql.org/docs/current/install-procedure.html)