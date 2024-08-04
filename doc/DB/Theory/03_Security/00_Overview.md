# 安全
# 概念
加密一般分为对称加密(Symmetric Key Encryption)和非对称加密(Asymmetric Key Encryption)。

对称加密又分为<b><font color=ff5533>分组加密和流密码。</font></b>

- 分组密码: 又叫块加密，将明文按照固定长度进行分组，然后对每个分组加密，每个分组经过一系列的数学变换生成相同长度的密文。典型的明文分组大小是64位或者128位。这种加密方式的特点是，它将整个明文组作为一个整体进行处理，使得整个组的每一个位都受到保护。分组密码通常采用多轮迭代的置换和代换操作来保护明文信息。这种方法能够提供较强的安全性，因为攻击者难以通过分析单个位来推断出明文信息。
- 流密码: 又叫序列加密，逐位处理明文, 明文的每一位都会与密钥流中的对应位进行异或运算生成密文。这个密钥流是由一个伪随机数生成器产生的，其长度与明文的长度相等。由于密钥流是逐位生成的，因此流密码的加密和解密过程可以连续进行，这使得它在网络通信和无线电通信等领域有广泛的应用。

<b><font color=ff5533>分组加密算法有ECB,CBC,CFB,OFB算法模式。</font></b>

- ECB(Electronic Code Book)/电码本模式
- CBC(Cipher Block Chaining)/密文分组链接方式
- Cipher Feedback (CFB)/密文反馈模式
- Output Feedback (OFB)/输出反馈模式

---

## 加密标准



高级加密标准(AES)：联邦信息处理标准 (FIPS) 197 中定义的对称式密码算法。AES提供3种经过批准的密钥长度：256、192和128位。



## 旋转根密钥

最好是定期轮换根密钥(即创建新版本的密钥)。 定期轮换减少了被称为密钥的 "cryptoperiod" ，也可用于人员更替，流程故障或检测安全问题等特定情况下。如果您怀疑某个密钥已泄露，可以禁用跟密钥。



[Mysql旋转密钥](../../MySQL/00_Overview.md#主密钥轮换)

## 透明加密TDE

[PolarDB: TDE透明数据加密](https://docs.polardbpg.com/1714370867366/features/v11/security/tde.html)
(OpenGuass:TDE)(https://docs-opengauss.osinfra.cn/zh/docs/3.0.0/docs/Developerguide/%E8%AE%BE%E7%BD%AE%E9%80%8F%E6%98%8E%E6%95%B0%E6%8D%AE%E5%8A%A0%E5%AF%86_TDE.html)

# 参考资料
1. [分组加密的四种模式(ECB、CBC、CFB、OFB)](https://www.cnblogs.com/yanzi-meng/p/9640578.html)
