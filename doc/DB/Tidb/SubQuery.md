
TiDB沿袭了SQL Server对子查询的处理思想，引入Apply算子将子查询采用子查询用代数形式表示，成为归一化，再根据Cost信息进行去关联。

# 参考资料
- [子查询相关优化](https://docs.pingcap.com/zh/tidb/stable/subquery-optimization)
- [TiDB 中的子查询优化技术](https://cn.pingcap.com/blog/tidb-optimization-for-subquery)
- [知乎:TiDB 源码学习：常见子查询优化](https://zhuanlan.zhihu.com/p/60931851)
