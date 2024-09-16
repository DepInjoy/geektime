# xargs命令
`xargs`将标准输入转为命令行参数，命令格式如下：
```shell
xargs [-options] [command]

支持的option有：
    -d:     更改分隔符
    -p:     打印出要执行的命令，询问用户是否要执行
    -t:     则是打印出最终要执行的命令，然后直接执行，不需要用户确认
    -L：    指定多少行作为一个命令行参数
```

例如根据月份删除core文件
```shell
# 删除6月份的core文件
ll -h core.* | grep Jun | awk -F " " '{print $9}' | xargs rm -fr
```

# awk命令

# 
netstat 是一个显示网络连接、路由表、接口统计、伪装连接等的工具。ss 是 netstat 的现代替代工具，速度更快，信息更加详细。ßßß
```shell
netstat -tulnp | grep :<端口号>
```
- `-t`：显示 TCP 连接。
- `-u`：显示 UDP 连接。
- `-l`：显示监听的套接字。
- `-n`：显示数字格式的地址和端口。
- `-p`：显示相关的进程 ID 和进程名称。
