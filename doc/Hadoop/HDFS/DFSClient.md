## 文件写操作



通过`DistributedFileSystem`的`create`方法来创建空HDFS文件

```java
  /**
   * @brief:    创建一个空HDFS文件,并返回文件输出对象，之后客户端可以通过其进行write进行写操作
   *
   * @return:   HDFS的文件输出对象HdfsDataOutputStream
   * */ 
public HdfsDataOutputStream create(final Path f,
      final FsPermission permission, final boolean overwrite,
      final int bufferSize, final short replication, final long blockSize,
      final Progressable progress, final InetSocketAddress[] favoredNodes)
      throws IOException {
    statistics.incrementWriteOps(1);
    storageStatistics.incrementOpCounter(OpType.CREATE);
    Path absF = fixRelativePart(f);
    return new FileSystemLinkResolver<HdfsDataOutputStream>() {
      @Override
      public HdfsDataOutputStream doCall(final Path p) throws IOException {
        // 通过DFClient.create方法来创建DFSOutputStream
        final DFSOutputStream out = dfs.create(getPathName(f), permission,
            overwrite ? EnumSet.of(CreateFlag.CREATE, CreateFlag.OVERWRITE)
                : EnumSet.of(CreateFlag.CREATE),
            true, replication, blockSize, progress, bufferSize, null,
            favoredNodes);
        // 构造DFSOutputStream并返回
        return safelyCreateWrappedOutputStream(out);
      }
      @Override
      public HdfsDataOutputStream next(final FileSystem fs, final Path p)
          throws IOException {
        if (fs instanceof DistributedFileSystem) {
          DistributedFileSystem myDfs = (DistributedFileSystem)fs;
          return myDfs.create(p, permission, overwrite, bufferSize, replication,
              blockSize, progress, favoredNodes);
        }
        throw new UnsupportedOperationException("Cannot create with" +
            " favoredNodes through a symlink to a non-DistributedFileSystem: "
            + f + " -> " + p);
      }
    }.resolve(this, absF);
  }
```

`DFClient`创建`DFSOutputStream`

```Java
  public DFSOutputStream create(String src, FsPermission permission,
      EnumSet<CreateFlag> flag, boolean createParent, short replication,
      long blockSize, Progressable progress, int buffersize,
      ChecksumOpt checksumOpt, InetSocketAddress[] favoredNodes,
      String ecPolicyName, String storagePolicy)
      throws IOException {
    // 1. 检查客户端是否打开
    checkOpen();
    final FsPermission masked = applyUMask(permission);
    LOG.debug("{}: masked={}", src, masked);
    /*
        2. 创建DFSOutputStream,其中DFSOutputStream提供了两个方法
          2.1 newStreamForCreate: 新文件构建DFSOutputStream输出流对象
          2.2 newStreamForAppend: 追加写一个已有文件时创建DFSOutputStream输出流对象
     */
    final DFSOutputStream result = DFSOutputStream.newStreamForCreate(this,
        src, masked, flag, createParent, replication, blockSize, progress,
        dfsClientConf.createChecksum(checksumOpt),
        getFavoredNodesStr(favoredNodes), ecPolicyName, storagePolicy);
    // 3. 注册获取文件租约
    beginFileLease(result.getFileId(), result);
    return result;
  }
```



```java

  /**
   * @brief:    新文件构建DFSOutputStream输出流对象
   * */
  static DFSOutputStream newStreamForCreate(DFSClient dfsClient, String src,
      FsPermission masked, EnumSet<CreateFlag> flag, boolean createParent,
      short replication, long blockSize, Progressable progress,
      DataChecksum checksum, String[] favoredNodes, String ecPolicyName,
      String storagePolicy)
      throws IOException {
    try (TraceScope ignored =
             dfsClient.newPathTraceScope("newStreamForCreate", src)) {
      HdfsFileStatus stat = null;

      // Retry the create if we get a RetryStartFileException up to a maximum
      // number of times
      boolean shouldRetry = true;
      int retryCount = CREATE_RETRY_COUNT;
      while (shouldRetry) {
        shouldRetry = false;
        try {
          // 1. 调用ClientProtocol发RPC给NN,在命名空间创建HDFS文件,返回申请文件的状态信息
          stat = dfsClient.namenode.create(src, masked, dfsClient.clientName,
              new EnumSetWritable<>(flag), createParent, replication,
              blockSize, SUPPORTED_CRYPTO_VERSIONS, ecPolicyName,
              storagePolicy);
          break;
        } catch (RemoteException re) {
          IOException e = re.unwrapRemoteException(....);
          if (e instanceof RetryStartFileException) {
            if (retryCount > 0) {
              shouldRetry = true;
              retryCount--;
            } else {
              throw new IOException("Too many retries because of encryption" + " zone operations", e);
            }
          } else {
            throw e;
          }
        }
      }
      Preconditions.checkNotNull(stat, "HdfsFileStatus should not be null!");
      final DFSOutputStream out;
      // 2. 是否区分开启EC特性,创建输出流
      if(stat.getErasureCodingPolicy() != null) {
        /**
         * 2.1 创建EC专用输出流, EC下创建List<StripedDataStreamer>
         *     该特性是:会选一个发速最快的streamer与NN发RPC, 然后通知其他的streamer结果
         */
        out = new DFSStripedOutputStream(dfsClient, src, stat,
            flag, progress, checksum, favoredNodes); // EC专用输出流
      } else {
        /**
         *  2.2 默认无EC,创建DFSOutputStream，该过程中
         *    2.2.1 通过params创建DataStreamer, 然后初始化Package
         * */
        out = new DFSOutputStream(dfsClient, src, stat,
            flag, progress, checksum, favoredNodes, true); //
      }
      // 3. 启动stream线程
      out.start();
      return out;
    }
  }
```



`DFSOutputStream`的构造函数

```java
 /** Construct a new output stream for creating a file.
   *
   * @param   shouldSyncBlock:    是否在关闭时将数据块持久化到磁盘
   * */
  protected DFSOutputStream(DFSClient dfsClient, String src,
      HdfsFileStatus stat, EnumSet<CreateFlag> flag, Progressable progress,
      DataChecksum checksum, String[] favoredNodes, boolean createStreamer) {
    this(dfsClient, src, flag, progress, stat, checksum);
    this.shouldSyncBlock = flag.contains(CreateFlag.SYNC_BLOCK);
    /* 1. 确定数据包packet的大小, 同时确定一个数据包中包含多少个校验块chunk
         	packetSize: 默认64K，dfs.client-write-packet-size配置属性
     */
    computePacketChunkSize(dfsClient.getConf().getWritePacketSize(),
        bytesPerChecksum);

    // 2. 创建输出文件流
    if (createStreamer) {
      streamer = new DataStreamer(stat, null, dfsClient, src, progress,
          checksum, cachingStrategy, byteArrayManager, favoredNodes,
          addBlockFlags);
    }
  }
```



```java
  protected void computePacketChunkSize(int psize, int csize) {
    /*
    *   bodySize = 65536 - ((4+2) + (9+9+2+5+2)) = 65503 (字节)
    *   PKT_MAX_HEADER_LEN = PKT_LENGTHS_LEN + MAX_PROTO_SIZE
    * */
    final int bodySize = psize - PacketHeader.PKT_MAX_HEADER_LEN;
    // 完整校验块的大小,包括校验块数据和校验块数据对应的校验和
    // chunkSize = 512 + 4 = 516 byte (也就是说上传1个512字节的文件, 实际占516字节)
    final int chunkSize = csize + getChecksumSize();
    // 每个数据包packet可以包含的校验块(校验和+数据)数量=65503/516=126 (个)
    chunksPerPacket = Math.max(bodySize/chunkSize, 1);
    // 数据包的大小,理论packet64K,实际大小 = 516 * 126 = 65016 (63.5K)
    packetSize = chunkSize*chunksPerPacket;
    DFSClient.LOG.debug("computePacketChunkSize: src={}, chunkSize={}, "
            + "chunksPerPacket={}, packetSize={}",
        src, chunkSize, chunksPerPacket, packetSize);
  }
```



`NameNode`的构造，`DFSClient`的构造函数中有如下的实现：

```java
    if (numResponseToDrop > 0) {
      // This case is used for testing.
      LOG.warn(DFS_CLIENT_TEST_DROP_NAMENODE_RESPONSE_NUM_KEY
          + " is set to " + numResponseToDrop
          + ", this hacked client will proactively drop responses");
      proxyInfo = NameNodeProxiesClient.createProxyWithLossyRetryHandler(conf,
          nameNodeUri, ClientProtocol.class, numResponseToDrop,
          nnFallbackToSimpleAuth);
    }

    if (proxyInfo != null) {
      this.dtService = proxyInfo.getDelegationTokenService();
      this.namenode = proxyInfo.getProxy();
    } else if (rpcNamenode != null) {
      // This case is used for testing.
      Preconditions.checkArgument(nameNodeUri == null);
      this.namenode = rpcNamenode;
      dtService = null;
    } else {
      Preconditions.checkArgument(nameNodeUri != null, "null URI");
      proxyInfo = NameNodeProxiesClient.createProxyWithClientProtocol(conf, nameNodeUri, nnFallbackToSimpleAuth);
      this.dtService = proxyInfo.getDelegationTokenService();
      this.namenode = proxyInfo.getProxy();
    }
```

