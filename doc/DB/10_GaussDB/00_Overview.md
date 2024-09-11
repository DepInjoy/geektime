# 安全
## 透明加密
<b><font color=FF5533>采用三层密钥结构实现密钥管理机制</font></b>，即根密钥(RK)、主密钥(CMK)和数据加密密钥(DEK)。主密钥由根密钥加密保护，数据加密密钥由主密钥加密保护。数据加密密钥用于对用户数据进行加密和解密，每个表对应一个数据加密密钥。根密钥和主密钥保存在KMS服务中，数据加密密钥通过向KMS服务申请创建，创建成功可同时返回密钥明文和密文。数据加密密钥明文在内存中会使用hash表进行缓存减少访问KMS频次以提升性能，密钥明文只存在内存中使用不会落盘，并且支持自动淘汰机制删除不常使用的密钥明文，只保存最近1天内使用的密钥明文。数据加密密钥密文保存在数据库中并落盘持久化。对用户表数据加解密时，如果内存中没有对应密钥明文则需向KMS申请对数据密钥解密后再使用。

---
GaussDB 3.3.0 支持表级密钥存储，实现对行存表加密，规格约束如下：
- 支持heap存储行存表加密。
- 不支持列存表加密，不支持物化视图加密，不支持ustore存储引擎加密
- 不支持索引和Sequence加密，<b><font color=FF5533>不支持XLOG日志加密</font></b>，不支持MOT内存表加密，不支持系统表加密。
---


### 表级加密方案
允许用户在创建表时指定是否对表进行加密和使用的加密算法，加密算法支持AES_128_CTR和SM4_CTR两种算法，算法一旦指定不可更改。对于创建表时指定为加密的表，数据库会自动为该表申请创建数据加密密钥，并将加密算法、密钥密文和对应主密钥ID等参数使用"keyword=value"格式保存在pg_class系统表中的reloptions字段中。

对于加密表，允许用户切换表的加密状态，即将加密表切换为非加密表，或将非加密表切换为加密表。如果在创建表时未使能加密功能，后续无法再切换为加密表。

对于加密表，支持数据加密密钥轮转。密钥轮转后，使用旧密钥加密的数据仍使用旧密钥解密，新写入的数据使用新密钥加密。密钥轮转时不更换加密算法。

对于行存表，每次加解密的最小数据单元为一个8K大小的page页面，每次对page页面加密时会通过安全随机数接口生成IV值，并将IV值和密钥密文、主密钥ID等信息保存在页面中一起写入存储介质。对于加密表由于page页面中需要保存加密密钥信息，相比不加密时占用存储空间膨胀约2.5%。

```sql
-- 创建加密表,加密状态为开启，指定AES_128_CTR加密算法
CREATE TABLE tde_test (a int, b text)
    with(enable_tde = on, encrypt_algo = 'AES_128_CTR');
-- 加密状态为开启，不指定加密算法，默认为AES_128_CTR加密算法
CREATE TABLE tde_test2 (a int, b text)
    with(enable_tde = on);
-- 加密状态为关闭，指定SM4_CTR加密算法
CREATE TABLE tde_test3 (a int, b text)
    with(enable_tde = off, encrypt_algo = 'SM4_CTR');


-- 切换加密开关
ALTER TABLE tde_test1 SET (enable_tde=on);


-- 密钥轮转
ALTER TABLE tde_test1 ENCRYPTION KEY ROTATION;
```

# 参考资料
1. [Gauss:设置透明数据加密(TDE)](https://docs-opengauss.osinfra.cn/zh/docs/3.0.0/docs/Developerguide/%E8%AE%BE%E7%BD%AE%E9%80%8F%E6%98%8E%E6%95%B0%E6%8D%AE%E5%8A%A0%E5%AF%86_TDE.html)
2. 