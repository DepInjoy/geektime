# ETL到数据联邦和数据虚拟化
> 数据联邦主要侧重于连接多个分散的异构自治系统，它的三个主要特征是：自治性、异质性和分布式。通常，数据联邦由一个主数据库进行管理，存储元数据。主数据库负责各个联邦数据库之间的通信，完成数据模型的映射。
> 
> 数据虚拟化则从用户角度出发，强调虚拟层的概念；对用户屏蔽底层的细节，提供统一的管理和访问接口。根据数据虚拟化的定义，无论外部数据存放在本地、S3、某个FTP服务器上，还是存放在某个独立自治的数据库上，用户在使用时不会感受到任何区别。
> 
> 由于二者的定义不在同一个抽象层次上，因此它们之间很难划分出明确界限。在很多场合，数据联邦与数据虚拟化被认为是等价的。
> 
> 在这里，我们根据Greenplum针对外部数据采取的操作方式，对Greenplum的数据联邦与数据虚拟化定义如下：数据联邦主要侧重Greenplum自身对多种异构数据源的连接能力，主要模块包括dblink、外部表(例如GPFDIST、S3、GPHDFS)，以及Gemfire连接器和Spark连接器。数据虚拟化则侧重通过提供中间层来屏蔽底层各个数据源对Greenplum的差异，中间层的主要代表有PXF(Platform Extension Framework)等[1]。

## GreenPlum的ETL工具
Greenplum继承了PostgreSQL的各种API接口和很多相关的第三方工具，例如，与客户端通信的libpq、JDBC以及ODBC等, PostgreSQL的ETL工具同样适用于Greenplum.

### PostgreSQL的ETL工具
- pg_dump是PostgreSQL的数据备份和恢复工具, 能够方便地将数据库备份为一个SQL文件.
- COPY是PostgreSQL官方的数据导入/导出工具, 可以方便地将数据库表导出为文本或者二进制文件.
- 通过JDBC/ODBC的方式实现与PostgreSQL的连接和数据加载，同样适用于Greenplum.
  
### GPLOAD
Greenplum推出了自己的高速加载工具GPLOAD. 

# 参考资料
- 《Greenplum：从大数据战略到实现》