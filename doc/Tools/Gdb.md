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

```shell
# 在特定函数设置断点
break function_name

# 在指定文件的某行设置断点
break file_name:line_number

# 执行当前行代码，并跳到下一行(不进入函数调用)
next

# 执行当前行代码，并进入函数调用内部
step

# 直接跳转到指定的代码行，继续执行
# 不会改变栈帧，也不会调用构造函数或析构函数
jump file_name:line_number

# 运行程序，直到到达指定行或者当前函数返回
# 通常用于在循环内跳过剩余的循环迭代。
until file_name:line_number

# 运行程序，直到当前函数返回，并在返回后停下来
finish
```