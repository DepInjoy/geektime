```shell
# gdb attach到指定进程,进行gdb调试
gdb attach <pid>
```

```shell
# 打印线程所有堆栈信息
thread apply all bt
```

从core文件获取线程信息
```shell
# 将输出重定向到文件，默认是gdb.txt
set logging on

# 关闭分页显示(快速打印)
set pagination off

# 打印所有线程信息
info threads
```

```shell
# 显示默认打印最大长度
show print elements;

# 不限制最大打印长度
print elements 0
```

postgres gdb调试
```shell
gdb pqsl

set args -p port -d db_name -U user_name
```