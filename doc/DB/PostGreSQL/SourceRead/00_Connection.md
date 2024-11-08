# 建立连接
## 安全信道:SSL
Postgres的`connectDBStart`实现和数据库建立连接，内部通过`conn->status`维护了一个状态机，从`CONNECTION_NEEDED`开始。下面主要关注建立SSL过程的相关实现，相关的状态如下，主要包含两部分：创建一个TCP Socket连接；SSL协商并完成SSL握手。
```
    CONNECTION_NEEDED
            | connect连接服务器
    CONNECTION_STARTED
            | getsockopt检查时候存在错误
            |
------------------------------------------------------------------------------------------
    CONNECTION_MADE     -- 成功建立一个裸Socket连接
            | 发送发送SSL请求NEGOTIATE_SSL_CODE包(须非域套接字)
            | 也就是startup package
    CONNECTION_SSL_STARTUP
            | 通过pqReadData获取server是否支持SSL
            | 如果支持SSL(SSLok = 'S')，不支持(SSLock = 'N')
            |   1. pqsecure_initialize(conn)
            |   2. pqsecure_open_client(conn)
            | 上面两步执行成功，conn->ssl_in_use = true
            | 返回PGRES_POLLING_WRITING
            | connectDBStart返回1建立连接成功
    CONNECTION_MADE   -- 成功建立SSL通信 
```
Postgre的连接实现是一个CS结构，
- Client:`src/interfaces/libpq/fe-connect.c`的`PQconnectPoll`在CONNECTION_MADE状态建立和Server的TCP连接。`src/interfaces/libpq/fe-secure.c`包含Client端尝试建立SSL连接
- Server:`src/backend/libpq/be-secure.c`包含Server端接受SSL连接，

### 建立TCP通信
Client创建TCP连接
```C
PostgresPollingStatusType PQconnectPoll(PGconn *conn) {
          ......
  switch (conn->status) {
    case CONNECTION_NEEDED:
    {
      sock_type = SOCK_STREAM;
      // 1. 创建socket
			conn->sock = socket(addr_cur->family, sock_type, 0);
      // 忽略一些socket option设置
          ......

      // 2. 创建连接,连接建立成功状态及切换到CONNECTION_STARTED
      //    返回PGRES_POLLING_WRITING，状态机继续推进
      if (connect(conn->sock, (struct sockaddr *) &addr_cur->addr.addr,
              addr_cur->addr.salen) < 0) {
        if (SOCK_ERRNO == EINPROGRESS ||
#ifdef WIN32
            SOCK_ERRNO == EWOULDBLOCK ||
#endif
            SOCK_ERRNO == EINTR) {
            conn->status = CONNECTION_STARTED;
            return PGRES_POLLING_WRITING;
        }
      } else {
        conn->status = CONNECTION_STARTED;
				goto keep_going;
      }
            .....
    }
    case CONNECTION_STARTED:
    {
      socklen_t	optlen = sizeof(optval);
      if (getsockopt(conn->sock, SOL_SOCKET, SO_ERROR,
            (char *) &optval, &optlen) == -1) {
                ......
      } else if (optval != 0) {
                ......
      }

      /* Fill in the client address */
      conn->laddr.salen = sizeof(conn->laddr.addr);
      if (getsockname(conn->sock, (struct sockaddr *)
            &conn->laddr.addr, &conn->laddr.salen) < 0) {
                  ......
      }
      conn->status = CONNECTION_MADE;
      return PGRES_POLLING_WRITING;
    }
                  ......
  }
}
```

Server端bind等待客户端连接
```C

```


### SSL协商和握手
