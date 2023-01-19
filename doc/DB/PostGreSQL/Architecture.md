# 进程架构

PostgreSQL是一个客户端/服务器风格的关系型数据库管理系统，采用多进程架构，运行在单台主机上。PostgreSQL Server实际上是一系列协同工作的进程集合，其中包含下列进程：

- Postgres服务器进程（postgres server process）是所有数据库集簇管理进程的父进程。
- 每个后端进程（backend process）负责处理客户端发出的查询和语句。
- 各种后台进程（background process）负责执行各种数据库管理任务（例如清理过程与存档过程）。
-  各种复制相关进程（replication associated process）负责流复制，流复制的细节会在第11章中介绍。
-  后台工作进程（background worker process）在9.3版本中被引入，它能执行任意由用户实现的处理逻辑。

<center>
	<img src="./img/PostGre-Process-Arch.png" width=75% height=75%>
	<div>不同资源组的查询占用CPU的实时对比</div>
</center>

## 服务器进程

postgres服务器进程是 PostgreSQL服务器中所有进程的父进程，在早期版本中被称为“postmaster”。带start参数执行pg_ctl实用程序会启动一个postgres服务器进程。它会在内存中分配共享内存区域，启动各种后台进程，如有必要还会启动复制相关进程与后台工作进程，并等待来自客户端的连接请求。每当接收到来自客户端的连接请求时，它都会启动一个后端进程，然后由启动的后端进程处理该客户端发出的所有查询。



## 后端进程

每个后端进程（也称为“postgres”）由 postgres 服务器进程启动，并处理连接另一侧的客户端发出的所有查询。它通过单条TCP连接与客户端通信，并在客户端断开连接时终止。因为一条连接只允许操作一个数据库，所以必须在连接到PostgreSQL服务器时显式地指定要连接的数据库。PostgreSQL允许多个客户端同时连接，配置参数max_connections用于控制最大客户端连接数（默认为100）。

因为 PostgreSQL 没有原生的连接池功能，所以如果许多客户端频繁地重复与 PostgreSQL服务器建立断开连接（譬如Web 应用），则会导致建立连接与创建后端进程的开销变大。这种情况对数据库服务器的性能有负面影响，通常可以使用池化中间件（pgbouncer或pgpool-II）来避免该问题。



## 后台进程

<center>
    <img src="./img/background-process.png" width=75% height=75%>
    <div>后台进程</div>
</center>



# 体系结构

物理结构由共享内存(shared memory)和少量的后台进程以及数据文件组成。

![](./img/pg-arch.png)

## 共享内存(Shared Memory)

共享内存是指提供数据缓存和事务日志缓存的内存。在共享内存中最重要的组成部分是共享缓冲区(`Shared Buffer`)和预写日志缓存(`WAL buffer`)。

### Shared Buffer

主要目的是最大限度的减少磁盘IO，为达到这个目的，必须要满足如下要求

- 需要快速访问非常大的缓冲区

- 当多个用户同时访问并发生争用时，应该确保最小化争用。
- 使用最频繁的数据块必须尽可能长时间的保存在缓冲区中。

### `WAL Buffer` 

`WAL Buffer`是一个临时存储数据更改的缓冲区，存储在`WAL Buffer`中的内容将在预定时间点写入`WAL`文件。从备份和恢复的角度，`WAL Buffer`和`WAL`文件非常重要。



## 后台进程

PostGreSQL有四种类型的进程

- Postmaster(Daemon)进程。

    PostGreSQL启动的第一个进程。负责实施恢复，初始化共享内存，并启动后台进程。当客户端进程有链接请求时，负责创建后端进程。

- 后台进程(Background Process)

- 后端进程(Backend Process)

- 客户端进程(Client Process)

![](./img/post-process.png)





# 数据库结构

## 表空间



## 表

> 什么是Vacuum?
>
> Vacuum做了如下这些事
>
> 1. 收集表和索引统计信息
> 2. 重新组织表
> 3. 清理表和索引无用的数据块
> 4. 由记录XID冻结，以防止XID循环
>
> 1和2通常是DBMS管理所必需的。但是3和4是必要的，因为PostgreSQL MVCC特性

# 参考资料

- [阿里云:理解PostgreSQL的体系结构](https://developer.aliyun.com/article/708764)