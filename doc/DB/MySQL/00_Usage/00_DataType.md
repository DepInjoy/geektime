# 数据类型

## 整数

| 数据类型    | 有符号取值范围                                 | 无符号取值范围                               | 占用字节数 |
| ----------- | ---------------------------------------------- | -------------------------------------------- | ---------- |
| `tinyint`   | `-128` ~ `127`                                 | `[0, 255]`                                   | 1          |
| `smallint`  | `-32768` ~ `32767`                             | `[0, 65535]`                                 | 2          |
| `mediumint` | `[-8388608, 8388607]`                          | `[0, 16777215]`                              | 3          |
| `int`       | `-2147483648`  ~ `2147483647`                  | `[0, 4294967295]`                            | 4          |
| `bigint`    | `-9223372036854775808` ~ `9223372036854775807` | `[0, 18446744073709551615]`<br/>`(2^64 - 1)` | 8          |



## 浮点数
MySQL支持的浮点数类型，分别是：
- `FLOAT`，表示单精度浮点数。
- `DOUBLE`，表示双精度浮点数。
- `REAL`，默认就是 DOUBLE，若把SQL模式设定为启用REAL_AS_FLOAT，那么，MySQL 就认为REAL是FLOAT。执行`SET sql_mode = “REAL_AS_FLOAT”;`来启用`REAL_AS_FLOAT`。

| 数据类型 | 有符号取值范围                                               | 无符号取值范围                                               | 占用字节数 |
| -------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---------- |
| `float`  | (`-3.402823466E+38` ~ `-1.175494351E-38`)<br/>0<br/>(`1.175494351E-38` ~ `3.402823466E+38`) | 0<br/>( `1.175494351E-38` ~ `3.402823466E+38`)               | 4          |
| `double` | （`-1.7976931348623157E+308` ~ `-2.2250738585072014E-308`)<br/> 0<br/>(`2.2250738585072014E-308`  ~ `1.7976931348623157E+308`) | 0<br/>(`2.2250738585072014E-308`  ~ `1.7976931348623157E+308`) | 8          |

> 为什么浮点数类型的无符号数取值范围，只相当于有符号数取值范围的一半，也就是只相当于有符号数取值范围大于等于零的部分呢？
>
> MySQL 是按照这个格式存储浮点数的：符号(S)、尾数(M)和阶码(E)。无论有没有符号，MySQL的浮点数都会存储表示符号的部分。无符号数取值范围，其实就是有符号数取值范围大于等于零的部分。

> MySQL 对浮点类型数据的存储方式会导致出现误差。
>
> MySQL 用 4 个字节存储 FLOAT 类型数据，用 8 个字节来存储 DOUBLE 类型数据。无论哪个，都是采用二进制的方式来进行存储的。比如 9.625，用二进制来表达，就是 1001.101或者表达成 1.001101×2^3。看到了吗？如果尾数不是 0 或 5(比如 9.624)，就无法用一个二进制数来精确表达。只好在取值允许的范围内进行近似（四舍五入）。FLOAT类型的误差会更大一些，而DOUBLE 有8字节，精度更高。

值得注意的是，MySQL官方文档[Problems with Floating-Point Values](https://dev.mysql.com/doc/refman/8.3/en/problems-with-float.html)

> 21.4 <> 21.4，可能为真
>
> because the difference between the numbers shows up around the tenth decimal or so, depending on factors such as computer architecture or the compiler version or optimization level. For example, different CPUs may evaluate floating-point numbers differently.
>
> 是说，浮点数值之间的差异在小数点后第十位左右，决定取决于计算机体系结构、编译器版本或优化级别等因素。例如，不同的 CPU评估(evaluate)浮点数的方式不同。

# 定点数据类型

MySQL 用`DECIMAL(M, D)`的方式表示高精度小数。其中，M表示整数部分加小数部分，一共有多少位，默认是10，M <= 65。D表示小数部分位数，D<M。

> DECIMAL 的存储方式决定了它一定是精准的。浮点数类型是把十进制数转换成二进制数存储，DECIMAL 则不同，它是把十进制数的整数部分和小数部分拆开，分别转换成十六进制数，进行存储。这样，所有的数值，就都可以精准表达了，不会存在因为无法表达而损失精度的问题。



# 参考资料

1. [极客时间-Mysql必知必会：02 | 字段：这么多字段类型，该怎么定义？](https://time.geekbang.org/column/article/350470?utm_campaign=geektime_search&utm_content=geektime_search&utm_medium=geektime_search&utm_source=geektime_search&utm_term=geektime_search)
2. [MySQL Doc : Problems with Floating-Point Values](https://dev.mysql.com/doc/refman/8.3/en/problems-with-float.html)

