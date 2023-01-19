# HDFS通信协议

ClientProtocol：ClientProtocol定义了客户端与名字节点间的接口，这个接口定义的方法非常多，客户端对文件系统的所有操作都需要通过这个接口，同时客户端读、写文件等操作也需要先通过这个接口与Namenode协商之后，再进行数据块的读出和写入操作。
■　ClientDatanodeProtocol：客户端与数据节点间的接口。ClientDatanodeProtocol中定义的方法主要是用于客户端获取数据节点信息时调用，而真正的数据读写交互则是通过流式接口进行

DatanodeProtocol：数据节点通过这个接口与名字节点通信，同时名字节点会通过这个接口中方法的返回值向数据节点下发指令。注意，这是名字节点与数据节点通信的唯一方式。这个接口非常重要，数据节点会通过这个接口向名字节点注册、汇报数据块的全量以及增量的存储情况。同时，名字节点也会通过这个接口中方法的返回值，将名字节点指令带回该数据块，根据这些指令，数据节点会执行数据块的复制、删除以及恢复操作。
■　InterDatanodeProtocol：数据节点与数据节点间的接口，数据节点会通过这个接口和其他数据节点通信。这个接口主要用于数据块的恢复操作，以及同步数据节点上存储的数据块副本的信息。
■　NamenodeProtocol：第二名字节点与名字节点间的接口。由于Hadoop2.X中引入了HA机制，检查点操作也不再由第二名字节点执行了，所以NamenodeProtocol我们就不详细介绍了。
■　其他接口：主要包括安全相关接口（RefreshAuthorizationPolicyProtocol、RefreshUser MappingsProtocol）、HA相关接口（HAServiceProtocol）等

ClientProtocol
ClientProtocol定义了所有由客户端发起的、由Namenode响应的操作。这个接口非常大，有80多个方法，我们把这个接口中的方法分为如下几类。
■　HDFS文件读相关的操作。
■　HDFS文件写以及追加写的相关操作。
■　管理HDFS命名空间（namespace）的相关操作。
■　系统问题与管理相关的操作。

照相关的操作。
■　缓存相关的操作。
■　其他操作。