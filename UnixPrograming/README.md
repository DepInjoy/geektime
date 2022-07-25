# Unix编程实现

## Socket编程
创建socket文件描述符
```C++
/**
 * @brief           1. create socket file descriptor
 * 
 * @param domain    communication domain
 *                        AF_INET : IPv4
 *                        AF_INET6 : IPv6
 * @param type      communication type
 *                        SOCK_STREAM : sequenced, reliable, two-way, connection-based byte streams
 *                                      eg:TCP
 *                        SOCK_DGRAM : datagrams (connectionless, unreliable messages of a fixed maximum length)
 *                                      eg:UDP
 * @param protocol  specify a particular Internet protocol(IP) to be used with the socket
 *                  Normally only a single protocol exists to support a particular socket type
 *                      within a given protocol family, which is 0
 */
int sockfd = socket(domain, type, protocol)
```
### TCP编程
[TCP编程实现](TCP/README.md)

### UDP编程

### 本地Socket编程


Always overcommit. Appropriate for some scientific
		applications. Classic example is code using sparse arrays
		and just relying on the virtual memory consisting almost
		entirely of zero pages.