Hadoop RPC内部实现位于源代码目录中hadoop-common-project/hadoop-common/src/main/java下的org.apache.hadoop.ipc包中，而YARN对RPC的ProtocolBuffers封装则位于hadoop-yarn-project/hadoop-yarn/hadoop-yarn-common/src/main/java目录下的org.apache.hadoop.yarn.ipc包中。

重点尝试取描述下面两个问题
- 客户端发送一个请求到接收到请求应答的整个过程是怎样的，依次经过了哪些函数调用和通信过程
- 多个客户端并发发送请求到服务器端后，服务器端是如何处理的

