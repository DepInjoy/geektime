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
## TCP编程
在创建Socket文件描述符时，要避免下面的写法
```C++
int server_fd;
if (server_fd = socket(AF_INET, SOCK_STREAM, 0) == 0) {
        close(server_fd);
        perror("Create socket failed!");
        exit(EXIT_FAILURE);
}
```
这样会导致在bind的时候会出现`socket operation on non-socket`，正确的写法是:
```C++
int server_fd;
if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        close(server_fd);
        perror("Create socket failed!");
        exit(EXIT_FAILURE);
}
```
注意添加括号。

### 参考资料
- [Socket Programming in C/C++](https://www.geeksforgeeks.org/socket-programming-cc/)
- [Socket Programming in C/C++: Handling multiple clients on server without multi threading](https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/)

## UDP编程

## 本地Socket编程
