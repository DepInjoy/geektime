# 安全

## 基本安全:密码管理

密码会过期，并且账户有可能会由于多次连续失败而被锁定。在修改密码之后，需要维护密码历史记录，以防止重用以前的密码。账户密码到期的特性是由指定给账户的配置文件决定的。配置文件由CREATE PROFILE命令创建，并由DBA(数据库管理员)管理。

关于密码和账户访问，配置文件规定了下列内容：
- 密码的生存期（lifetime），它决定了多长时间就必须更改密码。
- 密码的“过期日期”后的宽限期，在此期间可以修改密码。
- 在账户自动“锁定”之前允许连续失败的次数。
- 账户保持锁定状态的天数。
- 在重用一个密码前必须经过的天数。
- 在重用一个密码前该密码必须经历的更改次数。
- 附加的密码管理功能强制密码最小的长度和复杂性。

---

强制密码过期

可以使用配置文件来管理密码的过期、重用和复杂性。可以限定密码的使用期限，并锁定密码使用太久的账户。还可以强制密码至少为中等复杂，并将已经多次登录失败的账户锁住。
例如，如果将用户的配置文件中的FAILED_LOGIN_ATTEMPTS资源设置为5，则表示只允许该账户连续4次登录失败，第5次失败后该账户将被锁定。

要想将账户解锁，可以使用`ALTER USER`命令的`ACCOUNT UNLOCK`子句(用DBA账户)，如下所示：
```sql     
alter user jane account unlock;
```
在该账户解锁之后，将允许再次连接到jane账户。可以通过`ALTER USER`命令的`ACCOUNT LOCK`子句手工锁定一个账户。
```sql
alter user jane account lock;
```


如果一个账户由于多次连接失败而被锁定，则当超过配置文件设置的`PASSWORD _LOCK_TIME`值时，它将自动解锁。例如，如果`PASSWORD_LOCK_TIME`被设置为1，则上例中的jane账户将被锁定一天，一天之后该账户自动解锁。
可以通过配置文件中的`PASSWORD_LIFE_TIME`资源设置密码的最长使用期。例如，可以使用`LIMITED_PROFILE`配置文件强制用户每隔30天修改一次密码。
```sql
alter profile limited_profile limit password_life_time 30;
```
使用`ALTER PROFILE`命令来修改`LIMITED_PROFILE`配置文件。由于`PASSWORD_LIFE_TIME`的值设置为30，因此使用该配置文件的每个账户将在30天后密码到期。如果密码已经到期，则必须在下一次登录时修改它，除非配置文件对到期的密码有特定的宽限期。宽限期参数称为`PASSWORD_GRACE_TIME`。如果密码在宽限期内未修改，账户将过期。“过期”的账户与“锁定”的账户不同。锁定的账户可在一段时间后自动解锁，但过期的账户需要由DBA手工设置才能重新生效。

---

密码重用限制

为了防止密码被重用，可以使用配置文件中的两个参数：`PASSWORD_REUSE_MAX`或`PASSOWRD_REUSE_TIME`。这两个参数是相互排斥的，即如果设置了其中一个，则另一个必须设置为`UNLIMITED`。
- `PASSWORD_REUSE_TIME`参数指定在密码可以重用前必须间隔的天数。例如，如果设置`PASSWORD_REUSE_TIME`为60，则在60天内不能重用同样的密码。
- `PASSWORD_REUSE_MAX`参数指定在密码重用前密码必须变化的次数。如果试图在达到该限定值前重用该密码，Oracle将拒绝修改密码。
```sql
alter profile limited_profile limit
    password_reuse_max 3
    password_reuse_time unlimited;
```

## 高级安全性：虚拟数据库
虚拟专用数据库(Virtual Private Database，VPD）在应用程序的表之间提供记录级别的安全性。在VPD中，可以直接在表、视图和同义词上实施安全策略，这样用户就无法绕过安全设置了。


## 有限资源授权

在Oracle数据库中分配资源限额时，可以使用`CREATE USER`或`ALTER USER`命令中的`QUOTA`参数：
```sql
-- 给bob分配USERS表空间中100MB的限额
alter user bob quota 100g on users;
```
用户空间限额可在通过CREATE USER命令创建用户时设置。如果想撤消用户空间限额，以便不受限制，可以将`UNLIMITED TABLESPACE`系统权限授予该用户。

配置文件也可以限制其他的资源，例如规定用户对Oracle的请求可占用的CPU时间或空闲时间量。可创建详细规定这些资源限额的配置文件，并将之分配给一个或多个用户。



# 参考资料
1. 《Oracle Database 12c完全参考手册(第7版)》