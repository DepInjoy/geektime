# SQL查询

## SELECT语句
```sql
SELECT column1, column2, ...
FROM table_name
WHERE condition
GROUP BY column_name(s)
HAVING condition
ORDER [SIBLINGS] BY column1, column2, ... ASC|DESC;
```

## CASE-WHEN语句
```sql
CASE
    WHEN condition1 THEN result1
    WHEN condition2 THEN result2
    WHEN conditionN THEN resultN
    ELSE result
END;
```
# 数据类型
## 日期和时间类型

```sql
DATE
```

`DATE`类型用于包含日期部分但不包含时间部分。 MySQL 以“YYYY-MM-DD”格式检索和显示`DATE`类型的值。支持的范围是'1000-01-01'到'9999-12-31'。

---

```sql
DATETIME[(fsp)]
```
`DATETIME`类型用于包含日期和时间部分的值。 MySQL以'YYYY-MM-DD hh:mm:ss'格式检索并显示`DATETIME`类型的值。支持的范围是'1000-01-01 00:00:00' to '9999-12-31 23:59:59'.

---
```sql
TIMESTAMP[(fsp)]
```
`TIMESTAMP`数据类型用于包含日期和时间部分的值。`TIMESTAMP`的范围为'1970-01-01 00:00:01' UTC到'2038-01-19 03:14:07' UTC.。

`DATETIME`和`TIMESTAMP`精度可以到达微秒级(尾部6位小数)。插入`DATETIME`或 `TIMESTAMP`列的值小数部分都会被存储下来。包含小数部分后，格式为"YYYY-MM-DD hh:mm:ss[.fraction]"，`DATETIME`的范围为'1000-01-01 00:00:00.000000'到'9999 -12-31 23:59:59.499999'，`TIMESTAMP`的范围是 '1970-01-01 00:00:01.000000' 到'2038-01-19 03:14:07.499999'。

---

```sql
TIME[(fsp)]
```

取值范围是'-838:59:59.000000'到'838:59:59.000000'。 MySQL以'hh:mm:ss[.fraction]'格式显示`TIME`值，但允许使用字符串或数字将值给TIME列赋值。

### 计算函数

#### TIMEDIFF
```sql
TIMEDIFF(expr1,expr2)
```
返回TIME类型表示的`expr1 − expr2`。`expr1`和`expr2`是`string`转换为`TIME`或`DATETIME`类型；转换后它们必须具有相同的类型。如果`expr1`或`expr2`为 NULL，则返回`NULL`。

### 参考资料

[MySQL Doc: The DATE, DATETIME, and TIMESTAMP Types](https://dev.mysql.com/doc/refman/8.0/en/datetime.html)
[MySQL Doc: Date and Time Data Type Syntax](https://dev.mysql.com/doc/refman/8.0/en/date-and-time-type-syntax.html)
[MySQL Doc: Date and Time Functions](https://dev.mysql.com/doc/refman/8.0/en/date-and-time-functions.html#function_timediff)

