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

Server端调用accept等待客户端连接
```C
int StreamConnection(pgsocket server_fd, Port *port) {
    port->raddr.salen = sizeof(port->raddr.addr);
    if ((port->sock = accept(server_fd,
            (struct sockaddr *) &port->raddr.addr,
                &port->raddr.salen)) == PGINVALID_SOCKET) {
                        .....
		return STATUS_ERROR;
    }
                ......
	return STATUS_OK;
}
```


### SSL协商和握手

Server和SSL链接建立相关的实现在`src/backend/postmaster/postmaster.c`中，主要包含两部分：
1. `secure_initialize`加载SSL配置
2. `secure_open_server`
```c
#ifdef USE_SSL
    /* Reload SSL configuration as well */
    if (EnableSSL) { /**GUC配置ssl=on开启SSL*/
        if (secure_initialize(false) == 0)
            LoadedSSL = true;
        else
            ereport(LOG, (errmsg("SSL configuration was not reloaded")));
    } else {
        secure_destroy();
        LoadedSSL = false;
    }
#endif
```
```c
static int
ProcessStartupPacket(Port *port, bool ssl_done, bool gss_done) {
                        ......
    // 收到客户端请求建立SSL的消息
    if (proto == NEGOTIATE_SSL_CODE && !ssl_done) {
        char		SSLok;
#ifdef USE_SSL
                /* No SSL when disabled or on Unix sockets */
        if (!LoadedSSL || port->laddr.addr.ss_family == AF_UNIX)
            SSLok = 'N';
        else
            SSLok = 'S';    /* Support for SSL */
        #else
            SSLok = 'N';    /* No support for SSL */
#endif
        retry1:
            // 回复客户端是否支持SSL(S/N)
            if (send(port->sock, &SSLok, 1, 0) != 1) { ... }
#ifdef USE_SSL
            // 支持SSL, 等待SSL握手(SSL_accept)
            // 主要实现be_tls_open_server
            if (SSLok == 'S' && secure_open_server(port) == -1)
                return STATUS_ERROR;
#endif
        }
                        ......
	return STATUS_OK;
}
```

客户端和Server协商建立SSL也包含两个步骤：
1. `pqsecure_initialize`初始化SSL上下文
2. `pqsecure_open_client`和Server协商建立SSL连接，主要的实现是`pgtls_open_client`
```c
PostgresPollingStatusType PQconnectPoll(PGconn *conn) {
                ......
    switch (conn->status) {
        case CONNECTION_MADE: { // TCP连接已建立
#ifdef USE_SSL
        if (pqsecure_initialize(conn, false, true) < 0)
            goto error_return;

        if (conn->allow_ssl_try && !conn->wait_ssl_try && !conn->ssl_in_use
#ifdef ENABLE_GSS
                && !conn->gssenc
#endif
        ) {
            ProtocolVersion pv;
            // 向Server发送请求建立SSL消息包
            pv = pg_hton32(NEGOTIATE_SSL_CODE);
            if (pqPacketSend(conn, 0, &pv, sizeof(pv)) != STATUS_OK) {
                    libpq_append_conn_error(conn, "could not send SSL negotiation packet: %s",
                        SOCK_STRERROR(SOCK_ERRNO, sebuf, sizeof(sebuf)));
                goto error_return;
            }
            conn->status = CONNECTION_SSL_STARTUP;
            return PGRES_POLLING_READING;
        }
#endif
                 ......
        }
        case CONNECTION_SSL_STARTUP:
           if (!conn->ssl_in_use) {
                char		SSLok;
                int rdresult = pqReadData(conn);
                if (SSLok == 'S') {
                    conn->inStart = conn->inCursor;
                    // 1. 初始化SSL上下文
                    if (pqsecure_initialize(conn, true, false) != 0)
                        goto error_return;
                }
                        ......
           }
           // 2. 和Server协商建立SSL连接,主要实现在pgtls_open_client
           //    2.1 initialize_SSL创建SSL对象
           //    2.2 open_client_SSL执行SSL握手
           pollres = pqsecure_open_client(conn);
           if (pollres == PGRES_POLLING_OK) {
                // SSL连接建立成功，状态切换为CONNECTION_MADE
                conn->status = CONNECTION_MADE;
                return PGRES_POLLING_WRITING;
           }
                        ......
    }
}
```

### 发送数据
Server端发送数据
```c
ssize_t
secure_write(Port *port, void *ptr, size_t len) {
    ssize_t		n;
    int			waitfor;
                        ......
#ifdef USE_SSL
    if (port->ssl_in_use) {
        // 通过SSL_write发送消息
        n = be_tls_write(port, ptr, len, &waitfor);
    } else
#endif
    {
        //裸socket发送消息
        n = secure_raw_write(port, ptr, len);
        waitfor = WL_SOCKET_WRITEABLE;
    }
                ......
}
```

Client发送消息
```c
ssize_t
pqsecure_write(PGconn *conn, const void *ptr, size_t len) {
    ssize_t		n;
#ifdef USE_SSL
    if (conn->ssl_in_use) {
        // 通过SSL_write发送消息
        n = pgtls_write(conn, ptr, len);
    } else
#endif
    {
        // 裸socket发送消息
        n = pqsecure_raw_write(conn, ptr, len);
    }
    return n;
}
```

### 接受数据
Server端接受数据
```c
// src/backend/libpq/be-secure.c
ssize_t
secure_read(Port *port, void *ptr, size_t len) {
    ssize_t		n;
    int			waitfor;
                ......
retry:
#ifdef USE_SSL
    waitfor = 0;
    if (port->ssl_in_use) {
        // SSL_read接收数据
        n = be_tls_read(port, ptr, len, &waitfor);
    } else
#endif
    {
        // 通过裸socket接受数据
        n = secure_raw_read(port, ptr, len);
        waitfor = WL_SOCKET_READABLE;
    }
                ......
}
```

Client接收数据
```c
// src/interfaces/libpq/fe-secure.c
ssize_t
pqsecure_read(PGconn *conn, void *ptr, size_t len) {
    ssize_t		n;
#ifdef USE_SSL
    if (conn->ssl_in_use) {
        // SSL_read接收数据
        n = pgtls_read(conn, ptr, len);
    } else
#endif
    {
        // 通过裸socket接受数据
        n = pqsecure_raw_read(conn, ptr, len);
    }
    return n;
}
```

### 关闭连接
Server端关闭连接
```c
void
secure_close(Port *port) {
#ifdef USE_SSL
    if (port->ssl_in_use)
        be_tls_close(port);
#endif
}
```

Client关闭连接
```c
void
pqsecure_close(PGconn *conn) {
#ifdef USE_SSL
	pgtls_close(conn);
#endif
}
```