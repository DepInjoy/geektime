
# 加密标准
高级加密标准(AES)：联邦信息处理标准 (FIPS) 197 中定义的对称式密码算法。AES提供3种经过批准的密钥长度：256、192和128位。



# 旋转密钥

最好是定期轮换根密钥(即创建新版本的密钥)。 定期轮换减少了被称为密钥的 "cryptoperiod" ，也可用于人员更替，流程故障或检测安全问题等特定情况下。如果您怀疑某个密钥已泄露，可以禁用跟密钥。



[Mysql旋转密钥](../../MySQL/00_Overview.md#主密钥轮换)

## 透明加密TDE

[Mysql透明加密](../../MySQL/00_Overview.md#innodb静态数据加密)
[Oracle透明加密](../../Oracle/00_Overview.md#高级安全透明加密TDE)
[高斯DB透明加密](../../10_GauseDB/00_Overview.md#透明加密)
[OceanBase透明加密](../../10_OceanBase/00_overview.md#透明加密)