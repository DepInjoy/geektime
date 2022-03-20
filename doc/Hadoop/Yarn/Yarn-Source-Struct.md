# Yarn源码
Hadoop YARN源代码目录组织结构，总体上看，Hadoop YARN分为5部分：API、Common、Applications、Client和Server：
- YARN API（`hadoop-yarn-api`目录）：提供了YARN内部涉及的4个主要RPC协议的Java 声明和Protocol Buffers定义，这4个RPC协议分别是       
  - ApplicationClientProtocol
  - ApplicationMasterProtocol
  - ContainerManagementProtocol
  - ResourceManagerAdministrationProtocol 
- YARN Common（`hadoop-yarn-common`目录）：该部分包含了YARN底层库实现，包括事件库、服务库、状态机库、Web界面库等
- YARN Applications（`hadoop-yarn-applications`目录）：该部分包含了两个Application编程实例，分别是distributedshell和Unmanaged AM。
- YARN Client（`hadoop-yarn-client`目录）：该部分封装了几个与YARN RPC协议交互相关的库，方便用户开发应用程序。
- YARN Server（`hadoop-yarn-server`目录）：hadoop-yarn-api的实现，该部分给出了YARN的核心实现，包括ResourceManager、NodeManager、资源管理器等核心组件的实现
  - `hadoop-yarn-server-nodemanager`:
  - `hadoop-yarn-server-resourcemanager`:
  - hadoop-yarn-server-applicationhistoryservice
  - hadoop-yarn-server-common
  - hadoop-yarn-server.iml
  - hadoop-yarn-server-router
  - hadoop-yarn-server-sharedcachemanager
  - hadoop-yarn-server-timeline-pluginstorage
  - hadoop-yarn-server-timelineservice
  - hadoop-yarn-server-timelineservice-documentstore
  - hadoop-yarn-server-timelineservice-hbase
  - hadoop-yarn-server-web-proxy

- 服务库位于源代码hadoop-common-project/hadoop-common/src/main/java下的org.apache.hadoop.yarn.service包中。
- 事件库和状态机位于源代码hadoop-yarn-project/hadoop-yarn/hadoop-yarn-common/src/main/java下的org.apache.hadoop.yarn.event和org.apache.hadoop.yarn.state两个包中


